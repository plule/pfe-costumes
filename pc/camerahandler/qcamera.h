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
    QTimer *getWatchdog();
    QString getModel() const;
    QString getPort() const;
    bool isConnected() const;

    enum Status { OK, Error, Timeout, NotConnected };

protected:
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
    QThread m_camThread;
    QTimer *m_watchdog;
    QStringList m_errors;
    bool m_connected;

    int handleError(int error, QString msg);
    int buildCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

public slots:
    void captureToCamera(QString *camerapath);
    void captureToFile(const char *path, int nbTry=3);
    void captureToFile(QString path, int nbTry=3);
    void appendError(QString error);
    void _onTimeout();

protected slots:
    void _captureToFile(QString path, int nbTry=3);

signals:
    /* GPhoto2 signals */
    void idle();
    //	void info(const char *info, const char *camera);
    void error(QString error);
    void status(QString status);
    void message(QString message);
    void progress_start(QString task, int target);
    void progress_update(int current);
    void progress_stop();

    /* Custom signals */
    void finished(int status, QString path, QStringList errors);
    void connectionLost();

    /* Internal signals */
    void camera_answered();
    void wait_for_camera_answer();
};
}
#endif
