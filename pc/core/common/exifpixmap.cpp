#include "exifpixmap.h"

ExifPixmap::ExifPixmap()
{
}

QPixmap ExifPixmap::createPixmap(QString path)
{
    ExifData *exifData = exif_data_new_from_file(path.toStdString().c_str());
    int orientation = 0;
    if(exifData) {
        ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
        ExifEntry *exifEntry = exif_data_get_entry(exifData, EXIF_TAG_ORIENTATION);
        if(exifEntry) {
            orientation = exif_get_short(exifEntry->data, byteOrder);
        }
        exif_data_free(exifData);
    }
    QPixmap pic(path);
    /*
              0th Row      0th Column
           1  top          left side
           2  top          right side
           3  bottom       right side
           4  bottom       left side
           5  left side    top
           6  right side   top
           7  right side   bottom
           8  left side    bottom
        */
    QTransform t;
    switch(orientation) { // TODO : test flips
    case 1:
        break;
    case 2:
        t.scale(-1,1);
        break;
    case 3:
        t.rotate(180);
        break;
    case 4:
        t.rotate(180);
        t.scale(-1,1);
        break;
    case 5:
        t.rotate(90);
        t.scale(-1,1);
        break;
    case 6:
        t.rotate(90);
        break;
    case 7:
        t.rotate(-90);
        t.scale(-1,1);
        break;
    case 8:
        t.rotate(-90);
        break;
    default:
        break;
    }

    return pic.transformed(t);
}
