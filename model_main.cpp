#include "model_main.h"
#include "ui_model_main.h"

Model_main::Model_main(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Model_main)
{
    ui->setupUi(this);

    img.load(":/resouces/img/spichki.bmp");
    img = img.scaled(MAX_R, MAX_R);
    if (img.format() != QImage::Format_Grayscale8) {
        img = img.convertToFormat(QImage::Format_Grayscale8);
    }
    ui->label_pic_src->setPixmap(QPixmap::fromImage(img));


    grid = res = QImage(MAX_R, MAX_R, QImage::Format_Grayscale8);
    grid.fill(qRgb(255,255,255));
    res.fill(qRgb(255, 255, 255));
    ui->label_pic_grid->setPixmap(QPixmap::fromImage(grid));
    ui->label_pic_res->setPixmap(QPixmap::fromImage(res));
}

Model_main::~Model_main()
{
    delete ui;
}

void Model_main::on_pushButton_load_clicked()
{
//    QDir::homePath();
    QString imgFile = QFileDialog::getOpenFileName(this, tr("Открытие изображения"), "/Users/artemkaloev/faxmepic", tr("Image Files (*.gif *.tiff *.png *.jpg *.jpeg *.bmp)"));
    if (imgFile.isNull()) return;
    img.load(imgFile);

    img = img.scaled(MAX_R, MAX_R);
    if (img.format() != QImage::Format_Grayscale8) {
        img = img.convertToFormat(QImage::Format_Grayscale8);
    }
    ui->label_pic_src->setPixmap(QPixmap::fromImage(img));
}

void Model_main::process(int P)
{
    //очистить вектор полигонов
    polyVector.clear();
    //создать сетку
    polygon poly(0, 0, 256);
    polyVector.push_back(poly);
    poly.split(img, grid, polyVector, P);

    for (int i = 0; i < 4; i++) {
//        qDebug() << poly.children[i];
        if (poly.children[i])
            qDebug() << poly.children[i]->getY0();
    }
//    не работает????
//    qDebug() << (*poly.LD).getX0();
    //отобразить сетку
    ui->label_pic_grid->setPixmap(QPixmap::fromImage(grid));
    //отобразить количество полигонов
    ui->label_polyNum->setText(QString::number(polyVector.size()));
    //отобразить количество полигонов заданного размера
    on_spinBox_valueChanged(QString::number(ui->spinBox->value()));
    //формируем результирующее изображение и отображаем его
    formNewPic();
}

void Model_main::on_horizontalSlider_threshold_actionTriggered()
{
    grid.fill(qRgb(255, 255, 255));
    res.fill(qRgb(255, 255, 255));
    int P = ui->horizontalSlider_threshold->value();
    process(P);
    ui->spinBox_threshold->setValue(P);
}

void Model_main::on_spinBox_threshold_valueChanged(const QString &arg1)
{
    grid.fill(qRgb(255,255,255));
    res.fill(qRgb(255, 255, 255));
    int P = arg1.toInt();
    process(P);
    ui->horizontalSlider_threshold->setValue(P);
}

void Model_main::on_spinBox_valueChanged(const QString &arg1)
{
    int polySize = arg1.toInt();
    int counter = 0;
    for (int i = 0; i < polyVector.size(); i++) {
        if (polyVector[i].getR() == polySize) {
            counter++;
        }
    }
    ui->label_reqPolyNum->setText(QString::number(counter));
}

//void Model_main::split(int x0, int y0, int R, int P)
//{
//    polygon poly(x0, y0, R);
//    if (poly.reqSplit(img, P)) {
//        poly.isEmpty = false;
//        poly.draw(grid);
//        for (int x = x0; x <= x0 + R/2; x += R/2) {
//            for (int y = y0; y <= y0 + R/2; y += R/2) {
//                split(x, y, R/2, P);
//            }
//        }
//    } else {
//        poly.setIntensity(poly.getIntensityFromPic(img));
//    }
//    polyVector.push_back(poly);
//}

void Model_main::formNewPic()
{
//    qDebug() << polyVector[polyVector.size() - 1].getR() << polyVector[polyVector.size() - 2].getR() << polyVector[polyVector.size() - 3].getR() << polyVector[polyVector.size() - 4].getR();
    for (int k = 0; k < polyVector.size(); k++) {
        if (polyVector[k].isEmpty) {
            int x0 = polyVector[k].getX0();
            int y0 = polyVector[k].getY0();
            int R = polyVector[k].getR();
            int intensity = polyVector[k].getIntensity();
            for (int i = x0; i < x0 + R; i++) {
                for (int j = y0; j < y0 + R; j++) {
                    res.setPixel(i, j, qRgb(intensity, intensity, intensity));
                }
            }
        }
    }
    ui->label_pic_res->setPixmap(QPixmap::fromImage(res));
}
