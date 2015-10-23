#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QApplication>
#include <QStringList>
#include <QThread>
#include <gphoto2/gphoto2-camera.h>

//#define LEGACY_GPHOTO

#define R_GP_CALL(ret, fn, ...) ret = fn(__VA_ARGS__);\
    if(ret < GP_OK) {return handleError(ret, #fn );}

namespace QPhoto
{

/**
 * @brief The QCamera class handles a single camera
 */
class QCamera : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
public:
    QCamera();
    QCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);
    virtual ~QCamera();

    QString getSummary();
    QString getAbout();
    CameraAbilities getAbilities();

    /**
     * @brief getModel
     * @return Description of the camera model
     */
    QString getModel() const;
    /**
     * @brief getPort
     * @return Port name
     */
    QString getPort() const;

    /**
     * @brief isConnected
     * @return Check camera connection
     */
    bool isConnected() const;

    enum Status { OK, Error, Timeout, NotConnected };

    bool busy() const;

protected:
    /*
     * libgphoto context callback functions
     */
    friend void idle_func(GPContext *m_context, void *data);
    friend void progress_update_func(GPContext *m_context, unsigned int id, float current, void *data);
    friend void progress_stop_func(GPContext *m_context, unsigned int id, void *data);
#ifdef LEGACY_GPHOTO
    friend void error_func(GPContext *m_context, const char *format, va_list args, void *data);
    friend void status_func(GPContext *m_context, const char *format, va_list args, void *data);
    friend void message_func(GPContext *m_context, const char *format, va_list args, void *data);
    friend unsigned int progress_start_func(GPContext *m_context, float target, const char *format, va_list args, void *data);
#else
    friend void error_func(GPContext *context, const char *, void *data);
    friend void status_func(GPContext *context, const char *, void *data);
    friend void message_func(GPContext *context, const char *, void *data);
    friend unsigned int progress_start_func(GPContext *context, float target, const char *, void *data);
#endif
    int captureToQFile(QFile *file, int nbTry=3);

private:
    Camera *m_camera;
    GPContext *m_context;
    CameraAbilities m_abilities;
    QString m_model;
    QString m_port;
    QStringList m_errors;
    bool m_busy;
    bool m_connected;

    int handleError(int error, QString msg);
    int buildCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

public slots:
    /**
     * @brief captureToFile capture a photo to a file.
     * The format depends on camera's configuration.
     * @param path destination path
     * @param nbTry number of retry
     */
    int captureToFile(QString path, int nbTry=3);

    /**
     * @brief captureToCamera capture a photo and store it in camera's intern storage
     * @param nbTry number of retry
     * @return the path stored in the camera, or "" if capture failed
     */
    QPair<QString,QString> captureToCamera(int nbTry=3);

    /**
     * @brief getFile download a camera file to a local file
     * @param remoteFile path of the file of the camera
     * @param localFile file to download to
     * @return
     */
    int getFile(QString folder, QString name, QFile *localFile);

    /**
     * @brief deleteFile remove a file from camera's storage
     * @param folder
     * @param name
     * @return
     */
    int deleteFile(QString folder, QString name);

protected slots:
    //void _onTimeout();
    void appendError(QString error);

signals:
    /* GPhoto2 signals */
    /**
     * @brief idle the camera is idle and program can do anything else
     */
    void idle();
    //	void info(const char *info, const char *camera);
    /**
     * @brief The camera encountered an error described by the QString
     */
    void error(QString error);
    /**
     * @brief The camera status changed
     */
    void status(QString status);
    /**
     * @brief The camera said something
     */
    void message(QString message);
    /**
     * @brief progress_start The camera has started a new task. Target is the max value.
     * @param task
     * @param target
     */
    void progress_start(QString task, int target);
    /**
     * @brief progress_update The task progressed.
     * @param current
     */
    void progress_update(int current);
    /**
     * @brief progress_stop the task finished or stop
     */
    void progress_stop();

    /* Custom signals */
    /**
     * @brief finished is emited some time after calling a capture.
     * @param status indicate if the capture succeded
     * @param errors encountered errors
     */
    void finished(int status, QStringList errors);

    /**
     * @brief downloaded is emited after calling CaptureToFile when download is done or failed
     * @param status
     * @param errors
     */
    void downloaded(int status, QStringList errors);
};
}
#endif
