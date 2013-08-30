#ifndef EXIFPIXMAP_H
#define EXIFPIXMAP_H

#include <QPixmap>
#include <libexif/exif-data.h>

class ExifPixmap
{
public:
    ExifPixmap();
    static QPixmap createPixmap(QString path);

};

#endif // EXIFPIXMAP_H
