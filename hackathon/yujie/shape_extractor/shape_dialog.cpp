#include "shape_dialog.h"
#include "v3d_message.h"
#include <vector>
#include <math.h>
#include "string"
#include "sstream"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <basic_landmark.h>

shape_dialog::shape_dialog(V3DPluginCallback2 *cb)
{
    callback=cb;
    image1Dc_in=0;
    intype=0;
    sz_img[0]=sz_img[1]=sz_img[2]=sz_img[3]=0;
    LList.clear();
    create();

}

void shape_dialog::create()
{
    QBoxLayout *boxlayout=new QBoxLayout(QBoxLayout::TopToBottom);
    QToolButton *button_load=new QToolButton;
    button_load->setText("Load");
    button_load->setGeometry(0,0,10,20);
    QToolButton *button_extract=new QToolButton;
    button_extract->setText("Find edge");
    button_extract->setGeometry(0,0,10,20);
    QToolButton *button_return=new QToolButton;
    button_return->setText("Find geometric center");
    button_return->setGeometry(0,0,10,20);
    QToolButton *button_mark=new QToolButton;
    button_mark->setText("Mark");
    button_mark->setGeometry(0,0,10,20);
    QToolButton *button_para=new QToolButton;
    button_para->setText("Parameter setting");
    button_para->setGeometry(0,0,10,20);

    QToolBar *tool = new QToolBar;
    tool->setGeometry(0,0,200,20);
    tool->addWidget(button_load);
    tool->addSeparator();
    tool->addWidget(button_mark);
    tool->addSeparator();
    tool->addWidget(button_extract);
    tool->addSeparator();
    tool->addWidget(button_return);
    tool->addSeparator();
    tool->addWidget(button_para);
    //tool->setMovable(true);

    QHBoxLayout *vlayout = new QHBoxLayout;
    vlayout->addWidget(tool);

    QLabel *label= new QLabel;
    label->setText("Selected markers:");
    label->setMaximumHeight(50);

    edit=new QPlainTextEdit;
    edit->setPlainText("");
    edit->setReadOnly(true);
    edit->setFixedHeight(100);

    boxlayout->addLayout(vlayout);
    boxlayout->addWidget(label);
    boxlayout->addWidget(edit);
    setLayout(boxlayout);
    int height=tool->height()+label->height()+edit->height();
    this->setFixedHeight(height+80);

     connect(button_load, SIGNAL(clicked()), this, SLOT(load()));
     connect(button_mark,SIGNAL(clicked()),this, SLOT(loadMarkers()));
     connect(button_extract,SIGNAL(clicked()),this,SLOT(extract()));

}

bool shape_dialog::load()
{

    qDebug()<<"In load";
    QString fileName;
    fileName = QFileDialog::getOpenFileName(0, QObject::tr("Choose the input image "),
             QDir::currentPath(),QObject::tr("Images (*.raw *.tif *.lsm *.v3dpbd *.v3draw);;All(*)"));

    if (!fileName.isEmpty())
    {
        qDebug()<<"file is not empty";
        if (!simple_loadimage_wrapper(*callback, fileName.toStdString().c_str(), image1Dc_in, sz_img, intype))
        {
            QMessageBox::information(0,"","load image "+fileName+" error!");
            return false;
        }
        qDebug()<<"\t\timage size: [w="<<sz_img[0]<<", h="<<sz_img[1]<<", z="<<sz_img[2]<<", c="<<sz_img[3]<<"]";
        qDebug()<<"\t\tdatatype: "<<intype;
        V3DLONG size_tmp=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];

        if(intype==1)//V3D_UNIT8
        {
            shape_ext_obj.pushNewData<unsigned char>((unsigned char*)image1Dc_in, sz_img);
        }

        else if (intype == 2) //V3D_UINT16;
        {
            shape_ext_obj.pushNewData<unsigned short>((unsigned short*)image1Dc_in, sz_img);
            convert2UINT8((unsigned short*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else if(intype == 4) //V3D_FLOAT32;
        {
            shape_ext_obj.pushNewData<float>((float*)image1Dc_in, sz_img);
            convert2UINT8((float*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else
        {
           QMessageBox::information(0,"","Currently this program only supports UINT8, UINT16, and FLOAT32 data type.");
           return false;
        }

        qDebug()<<"before update input window";
        updateInputWindow();
        return true;
    }
    return false;

}


void shape_dialog::updateInputWindow()
{
    //search in open windows
    qDebug()<<"In updateinputwindow";
    bool winfound=false;
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
        {
            winfound=true;
        }
    }
    if(image1Dc_in != 0){ //image loaded
        //generate a copy and show it

        Image4DSimple image4d;
       // image4d.setFileName(this->edit_load->text().toStdString().c_str());
        V3DLONG size_page = sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
        unsigned char* image1D_input=memory_allocate_uchar1D(size_page);
        memcpy(image1D_input, image1Dc_in, size_page*sizeof(unsigned char));
        image4d.setData(image1D_input, sz_img[0], sz_img[1], sz_img[2], sz_img[3], V3D_UINT8);
        if(!winfound){ //open a window if none is open
            qDebug()<<"creat window";
            v3dhandle v3dhandle_main=callback->newImageWindow();
            //update the image
            callback->setImage(v3dhandle_main, &image4d);
            callback->setImageName(v3dhandle_main, NAME_INWIN);
            callback->updateImageWindow(v3dhandle_main);
            callback->open3DWindow(v3dhandle_main);
        }else{
//            qDebug()<<"refresh content";
            v3dhandleList_current=callback->getImageWindowList();
            for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
            {
                if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
                {
                    //update the image
                    if(!callback->setImage(v3dhandleList_current[i], &image4d)){
                        v3d_msg("Failed to update input image");
                    }
                    callback->updateImageWindow(v3dhandleList_current[i]);
                    //callback->open3DWindow(v3dhandleList_current[i]);
                    callback->pushImageIn3DWindow(v3dhandleList_current[i]);
                    callback->open3DWindow(v3dhandleList_current[i]);
                }
            }
        }
    }
 }

int shape_dialog::loadMarkers()
{
    qDebug()<<"in loadMarkers";
    LList.clear();
    QStringList cb_items;
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
        {
            LandmarkList LList_in = callback->getLandmark(v3dhandleList_current[i]);
            for(int i=0; i<LList_in.size(); i++){
                LList.append(LList_in.at(i));
                LList[i].color.r=196;
                LList[i].color.g=LList[i].color.b=0;
                cb_items.append("marker: " + QString::number(i+1));
            }

            break;

        }
    }

//    cb_marker->clear();
//    cb_marker->addItems(cb_items);
//    if(LList.size()>0){
//        extract();
//    }
//    checkButtons();
    qDebug()<<"posslandmark size"<<poss_landmark.size();
    qDebug()<<"Marker numbers:"<<LList.size();

    for (int i=0;i<LList.size();i++)
     {
        qDebug()<<"In the loop";
        QString tmp="Markers "+ QString::number(i+1)+ ": " + QString::number(LList[i].x)+","+
                QString::number(LList[i].y)+ "," +QString::number(LList[i].z);
        edit->appendPlainText(tmp);
     }

    return(LList.size());
}



void shape_dialog::extract()
{
    int convolute_iter=1;
    int neighbor_size=3;
    int bg_thr=60;
    poss_landmark=landMarkList2poss(LList, sz_img[0], sz_img[0]*sz_img[1]);
    image1Dc_out=memory_allocate_uchar1D(sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3]);
    memcpy(image1Dc_out,image1Dc_in, sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3]*sizeof(unsigned char));

    V3DLONG sumrsize=0;
    for (int j=0;j<poss_landmark.size();j++){
        V3DLONG rsize=shape_ext_obj.extract(x_all, y_all,z_all,
                                      poss_landmark[j], convolute_iter,neighbor_size, bg_thr);

        qDebug()<<"rsize:"<<rsize;
        if(rsize>0){

            V3DLONG finalpos;
            for (int i=0;i<x_all.size();i++)
            {
                finalpos=xyz2pos(x_all[i],y_all[i],z_all[i],sz_img[0],sz_img[0]*sz_img[1]);
                for (int z=0;z<sz_img[3];z++)
                image1Dc_out[finalpos+z*sz_img[0]*sz_img[1]*sz_img[2]]=255;
                sumrsize=rsize+sumrsize;
            }
        }
     }
    if (sumrsize>0){
    Image4DSimple image4d;
    v3dhandle v3dhandle_main=callback->newImageWindow();
    image4d.setData(image1Dc_out, sz_img[0], sz_img[1], sz_img[2], sz_img[3], V3D_UINT8);
    callback->setImage(v3dhandle_main, &image4d);
    callback->setLandmark(v3dhandle_main, LList);
    callback->setImageName(v3dhandle_main, NAME_OUTWIN);
    callback->updateImageWindow(v3dhandle_main);
    callback->open3DWindow(v3dhandle_main);
    callback->pushObjectIn3DWindow(v3dhandle_main);
    }

    else{
    v3d_msg("Nothing were found. Please change marker or adjust parameters.");}
}





//void shape_dialog::updateOutputWindow()
//{
//    //search in open windows
//    bool winfound=false;
//    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
//    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
//    {
//        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
//        {
//            winfound=true;
//        }
//    }
//    if(image1Dc_out != 0){ //image loaded
//        //generate a copy and show it
//        Image4DSimple image4d;
//        V3DLONG size_page = sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
//        unsigned char* image1Dc_input=neuronPickerMain::memory_allocate_uchar1D(size_page);
//        memcpy(image1Dc_input, image1Dc_out, size_page*sizeof(unsigned char));
//        image4d.setData(image1Dc_input, sz_img[0], sz_img[1], sz_img[2], sz_img[3], V3D_UINT8);

////        LandmarkList LList_single;
////        vector<V3DLONG> coord=extract_fun::pos2xyz(pos_out, sz_out[0], sz_out[0]*sz_out[1]);
////        LocationSimple Ltmp(coord[0],coord[1],coord[2]);
////        LList_single.append(Ltmp);

//        if(!winfound){ //open a window if none is open
//            v3dhandle v3dhandle_main=callback->newImageWindow();
//            callback->setImage(v3dhandle_main, &image4d);
////            callback->setLandmark(v3dhandle_main, LList_single);
//            callback->setImageName(v3dhandle_main, NAME_OUTWIN);
//            callback->updateImageWindow(v3dhandle_main);
//            callback->open3DWindow(v3dhandle_main);
//            callback->pushObjectIn3DWindow(v3dhandle_main);
//        }else{
//            //update the image
//            for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
//            {
//                if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
//                {
//                    callback->setImage(v3dhandleList_current[i], &image4d);
////                    callback->setLandmark(v3dhandleList_current[i], LList_single);
//                    callback->updateImageWindow(v3dhandleList_current[i]);
//                    //callback->open3DWindow(v3dhandleList_current[i]);
//                    callback->pushImageIn3DWindow(v3dhandleList_current[i]);
//                    callback->open3DWindow(v3dhandleList_current[i]);
//                    callback->pushObjectIn3DWindow(v3dhandleList_current[i]);
//                }
//            }
//        }
//    }
    /*else if(winfound){
        //no image loaded, but there is a window? clean up the window content
        Image4DSimple image4d;
        LandmarkList LList_empty;
        unsigned char *image1D_input=neuronPickerMain::memory_allocate_uchar1D(1);
        image1D_input[0]=0;
        image4d.setData(image1D_input,1,1,1,1,V3D_UINT8);
        for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
        {
            if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
            {
                callback->setImage(v3dhandleList_current[i], &image4d);
                callback->setLandmark(v3dhandleList_current[i], LList_empty);
                callback->updateImageWindow(v3dhandleList_current[i]);
                //callback->close3DWindow(v3dhandleList_current[i]);
                callback->pushImageIn3DWindow(v3dhandleList_current[i]);
                callback->pushObjectIn3DWindow(v3dhandleList_current[i]);
            }
        }
    }*/





void shape_dialog::convert2UINT8(unsigned short *pre1d, unsigned char *pPost, V3DLONG imsz)
{
    unsigned short* pPre = (unsigned short*)pre1d;
    unsigned short max_v=0, min_v = 255;
    for(V3DLONG i=0; i<imsz; i++)
    {
        if(max_v<pPre[i]) max_v = pPre[i];
        if(min_v>pPre[i]) min_v = pPre[i];
    }
    max_v -= min_v;
    if(max_v>0)
    {
        for(V3DLONG i=0; i<imsz; i++)
        {
            pPost[i] = (unsigned char) 255*(double)(pPre[i] - min_v)/max_v;
        }
    }
    else
    {
        for(V3DLONG i=0; i<imsz; i++)
        {
            pPost[i] = (unsigned char) pPre[i];
        }
    }
}

void shape_dialog::convert2UINT8(float *pre1d, unsigned char *pPost, V3DLONG imsz)
{
    float* pPre = (float*)pre1d;
    float max_v=0, min_v = 65535;
    for(V3DLONG i=0; i<imsz; i++)
    {
        if(max_v<pPre[i]) max_v = pPre[i];
        if(min_v>pPre[i]) min_v = pPre[i];
    }
    max_v -= min_v;
    if(max_v>0)
    {
        for(V3DLONG i=0; i<imsz; i++)
        {
            pPost[i] = (unsigned char) 255*(double)(pPre[i] - min_v)/max_v;
        }
    }
    else
    {
        for(V3DLONG i=0; i<imsz; i++)
        {
            pPost[i] = (unsigned char) pPre[i];
        }
    }
}