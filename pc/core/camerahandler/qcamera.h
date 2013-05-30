#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QStringList>
#include <gphoto2/gphoto2-camera.h>

#define LEGACY_GPHOTO

/*qDebug() << #fn;\*/
#define GP_CALL(ret, fn, ...) emit wait_for_camera_answer();\
    ret = fn(__VA_ARGS__);\
    emit camera_answered();

#define R_GP_CALL(ret, fn, ...) GP_CALL(ret, fn, __VA_ARGS__);\
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
     * @brief getWatchdog
     * @return The watchdog that will timeout when camera is lost
     */
    QTimer *getWatchdog();
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
    int _captureToFile(QFile *file);

private:
    Camera *m_camera;
    GPContext *m_context;
    CameraAbilities m_abilities;
    QString m_model;
    QString m_port;
    //GPPortInfo *portinfo;
    QThread m_camThread; // Each camera has its own thread to avoid global lock
    QTimer *m_watchdog; // Each camera has a watchdog to monitor potentially lockable function
    QStringList m_errors;
    bool m_connected;

    int handleError(int error, QString msg);
    int buildCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

public slots:
    void captureToCamera(QString *camerapath); // TODO delete ?
    /**
     * @brief captureToFile capture a photo to a file.
     * The format depends on camera's configuration.
     * Users should watch for the signal "finished" after calling this.
     * @param path destination path
     * @param nbTry number of retry
     */
    void captureToFile(const char *path, int nbTry=3);
    void captureToFile(QString path, int nbTry=3);

protected slots:
    void _onTimeout();
    void appendError(QString error);
    void _captureToFile(QString path, int nbTry=3);

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
     * @param path path of the saved photo (this path is the one given to captureToFile)
     * @param errors encountered errors
     */
    void finished(int status, QString path, QStringList errors);
    /**
     * @brief connectionLost This signal is emited when the camera stopped working.
     * The QCamera object is now useless and broken. You must disconnect and reconnect the camera
     * and recreate the QCamera object.
     */
    void connectionLost();

    /* Internal signals */
    void camera_answered();
    void wait_for_camera_answer();
};
}
#endif
