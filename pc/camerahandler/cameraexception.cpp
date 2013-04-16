#include "cameraexception.h"

namespace QPhoto
{
CameraException::CameraException(QString msg)
{
    m_msg = msg;
    msg.toStdString().c_str();
}

const char *CameraException::what() const throw()
{
    return m_msg.toStdString().c_str();
}
}
