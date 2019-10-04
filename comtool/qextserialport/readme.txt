使用方法：
pro文件
include(qextserialport/qextserialport.pri)

QextSerialPort *GSM_COM = new QextSerialPort(portName, QextSerialPort::EventDriven);
    isOpen = GSM_COM->open(QIODevice::ReadWrite);
    if (isOpen) {
        GSM_COM->flush();
        GSM_COM->setBaudRate(BAUD9600);
        GSM_COM->setDataBits(DATA_8);
        GSM_COM->setParity(PAR_NONE);
        GSM_COM->setStopBits(STOP_1);
        GSM_COM->setFlowControl(FLOW_OFF);
        GSM_COM->setTimeout(10);
    }

