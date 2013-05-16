#ifndef _QCAMERA_H_
#define _QCAMERA_H_

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <gphoto2/gphoto2-camera.h>

#include "cameraexception.h"

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
public:
    QCamera();
    QCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);
    virtual ~QCamera();

    QString getSummary();
    QString getAbout();
    CameraAbilities getAbilities();
    QTimer *getWatchdog();
    //void _captureToFile(QString path, int nbTry=3);


    QString getModel() const;

    QString getPort() const;

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
    int captureToFile(QFile *file);

private:
    Camera *m_camera;
    GPContext *m_context;
    CameraAbilities m_abilities;
    QString m_model;
    QString m_port;
    //GPPortInfo *portinfo;
    QThread m_camThread;
    QTimer *m_watchdog;


    int handleError(int error, QString msg);
    int buildCamera(const char *m_model, const char *m_port, CameraAbilitiesList *abilitiesList, GPPortInfoList *portinfolist);

public slots:
    void captureToCamera(QString *camerapath);
    void captureToFile(const char *path, int nbTry=3);
    void captureToFile(QString path, int nbTry=3);
    void _captureToFile(QString path, int nbTry=3);

signals:
    void idle();
    //	void info(const char *info, const char *camera);
    void error(QString error);
    void status(QString status);
    void message(QString message);
    void progress_start(QString task, int target);
    void progress_update(int current);
    void progress_stop();
    void captured(QString path);
    void operation_failed(QString msg);
    void operation_success();
    void camera_answered();
    void wait_for_camera_answer();
    void downloading();
};

class Sleeper : public QThread { // The horror...
public:
   void sleep(int ms) { QThread::sleep(ms); }
};
}
#endif
