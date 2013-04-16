#ifndef CAMERAEXCEPTION_H
#define CAMERAEXCEPTION_H

#include <exception>
#include <QString>

namespace QPhoto
{

//Q_ENUMS(ErrorCode)

class CameraException : public std::exception
{
public:
    CameraException(QString msg);
    virtual ~CameraException() throw(){}
    virtual const char * what() const throw();

private:
    QString m_msg;
};
}

#endif // CAMERAEXCEPTION_H
