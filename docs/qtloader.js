/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

// QtLoader provides javascript API for managing Qt application modules.
//
// QtLoader provides API on top of Emscripten which supports common lifecycle
// tasks such as displaying placeholder content while the module downloads,
// handing application exits, and checking for browser wasm support.
//
// There are two usage modes:
//  * Managed:  QtLoader owns and manages the HTML display elements like
//              the loader and canvas.
//  * External: The embedding HTML page owns the display elements. QtLoader
//              provides event callbacks which the page reacts to.
//
// Managed mode usage:
//
//     var config = {
//         containerElements : [$("container-id")];
//     }
//     var qtLoader = QtLoader(config);
//     qtLoader.loadEmscriptenModule("applicationName");
//
// External mode.usage:
//
//    var config = {
//        canvasElements : [$("canvas-id")],
//        showLoader: function() {
//            loader.style.display = 'block'
//            canvas.style.display = 'hidden'
//        },
//        showCanvas: function() {
//            loader.style.display = 'hidden'
//            canvas.style.display = 'block'
//            return canvas;
//        }
//     }
//     var qtLoader = QtLoader(config);
//     qtLoader.loadEmscriptenModule("applicationName");
//
// Config keys
//
//  containerElements : [container-element, ...]
//      One or more HTML elements. QtLoader will display loader elements
//      on these while loading the applicaton, and replace the loader with a
//      canvas on load complete.
//  canvasElements : [canvas-element, ...]
//      One or more canvas elements.
//  showLoader : function(status, containerElement)
//      Optional loading element constructor function. Implement to create
//      a custom loading screen. This function may be called multiple times,
//      while preparing the application binary. "status" is a string
//      containing the loading sub-status, and may be either "Downloading",
//      or "Compiling". The browser may be using streaming compilation, in
//      which case the wasm module is compiled during downloading and the
//      there is no separate compile step.
//  showCanvas : function(containerElement)
//      Optional canvas constructor function. Implement to create custom
//      canvas elements.
//  showExit : function(crashed, exitCode, containerElement)
//      Optional exited element constructor function.
//  showError : function(crashed, exitCode, containerElement)
//      Optional error element constructor function.
//
//  path : <string>
//      Prefix path for wasm file, realative to the loading HMTL file.
//  restartMode : "DoNotRestart", "RestartOnExit", "RestartOnCrash"
//      Controls whether the application should be reloaded on exits. The default is "DoNotRestart"
//  restartType : "RestartModule", "ReloadPage"
//  restartLimit : <int>
//     Restart attempts limit. The default is 10.
//  stdoutEnabled : <bool>
//  stderrEnabled : <bool>
//  environment : <object>
//     key-value environment variable pairs.
//
// QtLoader object API
//
// webAssemblySupported : bool
// webGLSupported : bool
// canLoadQt : bool
//      Reports if WebAssembly and WebGL are supported. These are requirements for
//      running Qt applications.
// loadEmscriptenModule(applicationName)
//      Loads the application from the given emscripten javascript module file and wasm file
// status
//      One of "Created", "Loading", "Running", "Exited".
// crashed
//      Set to true if there was an unclean exit.
// exitCode
//      main()/emscripten_force_exit() return code. Valid on status change to
//      "Exited", iff crashed is false.
// exitText
//      Abort/exit message.
// addCanvasElement
//      Add canvas at run-time. Adds a corresponding QScreen,
// removeCanvasElement
//      Remove canvas at run-time. Removes the corresponding QScreen.
// resizeCanvasElement
//      Signals to the application that a canvas has been resized.
// setFontDpi
//      Sets the logical font dpi for the application.


var Module = {}

function QtLoader(config)
{
    function webAssemblySupported() {
        return typeof WebAssembly !== "undefined"
    }

    function webGLSupported() {
        // We expect that WebGL is supported if WebAssembly is; however
        // the GPU may be blacklisted.
        try {
            var canvas = document.createElement("canvas");
            return !!(window.WebGLRenderingContext && (canvas.getContext("webgl") || canvas.getContext("experimental-webgl")));
        } catch (e) {
            return false;
        }
    }

    function canLoadQt() {
        // The current Qt implementation requires WebAssembly (asm.js is not in use),
        // and also WebGL (there is no raster fallback).
        return webAssemblySupported() && webGLSupported();
    }

    function removeChildren(element) {
        while (element.firstChild) element.removeChild(element.firstChild);
    }

    function createCanvas() {
        var canvas = document.createElement("canvas");
        canvas.className = "QtCanvas";
        canvas.style.height = "100%";
        canvas.style.width = "100%";

        // Set contentEditable in order to enable clipboard events; hide the resulting focus frame.
        canvas.contentEditable = true;
        canvas.style.outline = "0px solid transparent";
        canvas.style.caretColor = "transparent";
        canvas.style.cursor = "default";

        return canvas;
    }

    // Set default state handler functions and create canvases if needed
    if (config.containerElements !== undefined) {

        config.canvasElements = config.containerElements.map(createCanvas);

        config.showError = config.showError || function(errorText, container) {
            removeChildren(container);
            var errorTextElement = document.createElement("text");
            errorTextElement.className = "QtError"
            errorTextElement.innerHTML = errorText;
            return errorTextElement;
        }

        config.showLoader = config.showLoader || function(loadingState, container) {
            removeChildren(container);
            var loadingText = document.createElement("text");
            loadingText.className = "QtLoading"
            loadingText.innerHTML = '<p><center> ${loadingState}...</center><p>';
            return loadingText;
        };

        config.showCanvas = config.showCanvas || function(canvas, container) {
            removeChildren(container);
        }

        config.showExit = config.showExit || function(crashed, exitCode, container) {
            if (!crashed)
                return undefined;

            removeChildren(container);
            var fontSize = 54;
            var crashSymbols = ["\u{1F615}", "\u{1F614}", "\u{1F644}", "\u{1F928}", "\u{1F62C}",
                                "\u{1F915}", "\u{2639}", "\u{1F62E}", "\u{1F61E}", "\u{1F633}"];
            var symbolIndex = Math.floor(Math.random() * crashSymbols.length);
            var errorHtml = `<font size='${fontSize}'> ${crashSymbols[symbolIndex]} </font>`
            var errorElement = document.createElement("text");
            errorElement.className = "QtExit"
            errorElement.innerHTML = errorHtml;
            return errorElement;
        }
    }

    config.restartMode = config.restartMode || "DoNotRestart";
    config.restartLimit = config.restartLimit || 10;

    if (config.stdoutEnabled === undefined) config.stdoutEnabled = true;
    if (config.stderrEnabled === undefined) config.stderrEnabled = true;

    // Make sure config.path is defined and ends with "/" if needed
    if (config.path === undefined)
        config.path = "";
    if (config.path.length > 0 && !config.path.endsWith("/"))
        config.path = config.path.concat("/");

    if (config.environment === undefined)
        config.environment = {};

    var publicAPI = {};
    publicAPI.webAssemblySupported = webAssemblySupported();
    publicAPI.webGLSupported = webGLSupported();
    publicAPI.canLoadQt = canLoadQt();
    publicAPI.canLoadApplication = canLoadQt();
    publicAPI.status = undefined;
    publicAPI.loadEmscriptenModule = loadEmscriptenModule;
    publicAPI.addCanvasElement = addCanvasElement;
    publicAPI.removeCanvasElement = removeCanvasElement;
    publicAPI.resizeCanvasElement = resizeCanvasElement;
    publicAPI.setFontDpi = setFontDpi;
    publicAPI.fontDpi = fontDpi;

    restartCount = 0;

    function fetchResource(filePath) {
        var fullPath = config.path + filePath;
        return fetch(fullPath).then(function(response) {
            if (!response.ok) {
                self.error = response.status + " " + response.statusText + " " + response.url;
                setStatus("Error");
                return Promise.reject(self.error)
            } else {
                return response;
            }
        });
    }

    function fetchText(filePath) {
        return fetchResource(filePath).then(function(response) {
            return response.text();
        });
    }

    function fetchThenCompileWasm(response) {
        return response.arrayBuffer().then(function(data) {
            self.loaderSubState = "Compiling";
            setStatus("Loading") // trigger loaderSubState udpate
            return WebAssembly.compile(data);
        });
    }

    function fetchCompileWasm(filePath) {
        return fetchResource(filePath).then(function(response) {
            if (typeof WebAssembly.compileStreaming !== "undefined") {
                self.loaderSubState = "Downloading/Compiling";
                setStatus("Loading");
                return WebAssembly.compileStreaming(response).catch(function(error) {
                    // compileStreaming may/will fail if the server does not set the correct
                    // mime type (application/wasm) for the wasm file. Fall back to fetch,
                    // then compile in this case.
                    return fetchThenCompileWasm(response);
                });
            } else {
                // Fall back to fetch, then compile if compileStreaming is not supported
                return fetchThenCompileWasm(response);
            }
        });
    }

    function loadEmscriptenModule(applicationName) {

        // Loading in qtloader.js goes through four steps:
        // 1) Check prerequisites
        // 2) Download resources
        // 3) Configure the emscripten Module object
        // 4) Start the emcripten runtime, after which emscripten takes over

        // Check for Wasm & WebGL support; set error and return before downloading resources if missing
        if (!webAssemblySupported()) {
            self.error = "Error: WebAssembly is not supported"
            setStatus("Error");
            return;
        }
        if (!webGLSupported()) {
            self.error = "Error: WebGL is not supported"
            setStatus("Error");
            return;
        }

        // Continue waiting if loadEmscriptenModule() is called again
        if (publicAPI.status == "Loading")
            return;
        self.loaderSubState = "Downloading";
        setStatus("Loading");

        // Fetch emscripten generated javascript runtime
        var emscriptenModuleSource = undefined
        var emscriptenModuleSourcePromise = fetchText(applicationName + ".js").then(function(source) {
            emscriptenModuleSource = source
        });

        // Fetch and compile wasm module
        var wasmModule = undefined;
        var wasmModulePromise = fetchCompileWasm(applicationName + ".wasm").then(function (module) {
            wasmModule = module;
        });

        // Wait for all resources ready
        Promise.all([emscriptenModuleSourcePromise, wasmModulePromise]).then(function(){
            completeLoadEmscriptenModule(applicationName, emscriptenModuleSource, wasmModule);
        }).catch(function(error) {
            self.error = error;
            setStatus("Error");
        });
    }

    function completeLoadEmscriptenModule(applicationName, emscriptenModuleSource, wasmModule) {

        // The wasm binary has been compiled into a module during resource download,
        // and is ready to be instantiated. Define the instantiateWasm callback which
        // emscripten will call to create the instance.
        Module.instantiateWasm = function(imports, successCallback) {
            WebAssembly.instantiate(wasmModule, imports).then(function(instance) {
                successCallback(instance, wasmModule);
            }, function(error) {
                self.error = error;
                setStatus("Error");
            });
            return {};
        };

        Module.locateFile = Module.locateFile || function(filename) {
            return config.path + filename;
        };

        // Attach status callbacks
        Module.setStatus = Module.setStatus || function(text) {
            // Currently the only usable status update from this function
            // is "Running..."
            if (text.startsWith("Running"))
                setStatus("Running");
        };
        Module.monitorRunDependencies = Module.monitorRunDependencies || function(left) {
          //  console.log("monitorRunDependencies " + left)
        };

        // Attach standard out/err callbacks.
        Module.print = Module.print || function(text) {
            if (config.stdoutEnabled)
                console.log(text)
        };
        Module.printErr = Module.printErr || function(text) {
            // Filter out OpenGL getProcAddress warnings. Qt tries to resolve
            // all possible function/extension names at startup which causes
            // emscripten to spam the console log with warnings.
            if (text.startsWith !== undefined && text.startsWith("bad name in getProcAddress:"))
                return;

            if (config.stderrEnabled)
                console.log(text)
        };

        // Error handling: set status to "Exited", update crashed and
        // exitCode according to exit type.
        // Emscripten will typically call printErr with the error text
        // as well. Note that emscripten may also throw exceptions from
        // async callbacks. These should be handled in window.onerror by user code.
        Module.onAbort = Module.onAbort || function(text) {
            publicAPI.crashed = true;
            publicAPI.exitText = text;
            setStatus("Exited");
        };
        Module.quit = Module.quit || function(code, exception) {
            if (exception.name == "ExitStatus") {
                // Clean exit with code
                publicAPI.exitText = undefined
                publicAPI.exitCode = code;
            } else {
                publicAPI.exitText = exception.toString();
                publicAPI.crashed = true;
            }
            setStatus("Exited");
        };

        // Set environment variables
        Module.preRun = Module.preRun || []
        Module.preRun.push(function() {
            for (var [key, value] of Object.entries(config.environment)) {
                ENV[key.toUpperCase()] = value;
            }
        });

        Module.mainScriptUrlOrBlob = new Blob([emscriptenModuleSource], {type: 'text/javascript'});

        Module.qtCanvasElements = config.canvasElements;

        config.restart = function() {

            // Restart by reloading the page. This will wipe all state which means
            // reload loops can't be prevented.
            if (config.restartType == "ReloadPage") {
                location.reload();
            }

            // Restart by readling the emscripten app module.
            ++self.restartCount;
            if (self.restartCount > config.restartLimit) {
                self.error = "Error: This application has crashed too many times and has been disabled. Reload the page to try again."
                setStatus("Error");
                return;
            }
            loadEmscriptenModule(applicationName);
        };

        publicAPI.exitCode = undefined;
        publicAPI.exitText = undefined;
        publicAPI.crashed = false;

        // Finally evaluate the emscripten application script, which will
        // reference the global Module object created above.
        self.eval(emscriptenModuleSource); // ES5 indirect global scope eval
    }

    function setErrorContent() {
        if (config.containerElements === undefined) {
            if (config.showError !== undefined)
                config.showError(self.error);
            return;
        }

        for (container of config.containerElements) {
            var errorElement = config.showError(self.error, container);
            container.appendChild(errorElement);
        }
    }

    function setLoaderContent() {
        if (config.containerElements === undefined) {
            if (config.showLoader !== undefined)
                config.showLoader(self.loaderSubState);
            return;
        }

        for (container of config.containerElements) {
            var loaderElement = config.showLoader(self.loaderSubState, container);
            container.appendChild(loaderElement);
        }
    }

    function setCanvasContent() {
        if (config.containerElements === undefined) {
            if (config.showCanvas !== undefined)
                config.showCanvas();
            return;
        }

        for (var i = 0; i < config.containerElements.length; ++i) {
            var container = config.containerElements[i];
            var canvas = config.canvasElements[i];
            config.showCanvas(canvas, container);
            container.appendChild(canvas);
        }
    }

    function setExitContent() {

        // publicAPI.crashed = true;

        if (publicAPI.status != "Exited")
            return;

        if (config.containerElements === undefined) {
            if (config.showExit !== undefined)
                config.showExit(publicAPI.crashed, publicAPI.exitCode);
            return;
        }

        if (!publicAPI.crashed)
            return;

        for (container of config.containerElements) {
            var loaderElement = config.showExit(publicAPI.crashed, publicAPI.exitCode, container);
            if (loaderElement !== undefined)
                container.appendChild(loaderElement);
        }
    }

    var committedStatus = undefined;
    function handleStatusChange() {
        if (publicAPI.status != "Loading" && committedStatus == publicAPI.status)
            return;
        committedStatus = publicAPI.status;

        if (publicAPI.status == "Error") {
            setErrorContent();
        } else if (publicAPI.status == "Loading") {
            setLoaderContent();
        } else if (publicAPI.status == "Running") {
            setCanvasContent();
        } else if (publicAPI.status == "Exited") {
            if (config.restartMode == "RestartOnExit" ||
                config.restartMode == "RestartOnCrash" && publicAPI.crashed) {
                    committedStatus = undefined;
                    config.restart();
            } else {
                setExitContent();
            }
        }

        // Send status change notification
        if (config.statusChanged)
            config.statusChanged(publicAPI.status);
    }

    function setStatus(status) {
        if (status != "Loading" && publicAPI.status == status)
            return;
        publicAPI.status = status;

        window.setTimeout(function() { handleStatusChange(); }, 0);
    }

    function addCanvasElement(element) {
        if (publicAPI.status == "Running")
            Module.qtAddCanvasElement(element);
        else
            console.log("Error: addCanvasElement can only be called in the Running state");
    }

    function removeCanvasElement(element) {
        if (publicAPI.status == "Running")
            Module.qtRemoveCanvasElement(element);
        else
            console.log("Error: removeCanvasElement can only be called in the Running state");
    }

    function resizeCanvasElement(element) {
        if (publicAPI.status == "Running")
            Module.qtResizeCanvasElement(element);
    }

    function setFontDpi(dpi) {
        Module.qtFontDpi = dpi;
        if (publicAPI.status == "Running")
            Module.qtSetFontDpi(dpi);
    }

    function fontDpi() {
        return Module.qtFontDpi;
    }

    setStatus("Created");

    return publicAPI;
}
