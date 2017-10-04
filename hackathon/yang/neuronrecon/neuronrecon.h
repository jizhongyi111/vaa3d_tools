// "optimal constructing of neuron trees from voxels"
// -Yang
// 09/12/2017


#ifndef _NEURONRECON_H_
#define _NEURONRECON_H_

//
#include <QtGlobal>
#include <vector>
#include "v3d_interface.h"
#include "basic_surf_objs.h"
#include "v3d_message.h"
#include <iostream>
#include "basic_4dimage.h"
#include "algorithm"
#include <string.h>
#include <cmath>
#include <climits>
#include <numeric>
#include <algorithm>
#include <string>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkTimeProbe.h"
#include "itkImageRegionIterator.h"

#include "itkOpenCLUtil.h"
#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUGradientAnisotropicDiffusionImageFilter.h"

using namespace std;

//
class Point
{
public:
    Point();
    ~Point();

public:
    void setLocation(float x, float y, float z);
    void setRadius(float r);
    void setValue(float v);

public:
    float x, y, z; // location
    float radius; // radius
    float val; // intensity value
    vector<V3DLONG> parents;
    vector<V3DLONG> children;
    V3DLONG n; // #
    bool visited;
    int type; // -1 cell body; 0 tip point; 1 regular point; 3 branch point
};

//
class PointCloud
{
public:
    PointCloud();
    ~PointCloud();

public:
    int getPointCloud(QStringList files);
    int savePointCloud(QString filename);
    int savePC2SWC(PointCloud pc, QString filename);
    int addPointFromNeuronTree(NeuronTree nt);

    float distance(Point a, Point b);

    int getBranchPoints(QString filename);
    int getNeurites(QString filename);

    int resample();

    // statistics
    float getAngle(Point a, Point b, Point c);

public:
    vector<Point> points;
};

class Quadruple
{
public:
    Quadruple();
    ~Quadruple();

public:
    int find3nearestpoints(Point p, PointCloud pc);

public:
    vector<Point> quad;
    float alpha, beta, theta;

};

class LineSegment : public PointCloud
{
public:
    LineSegment();
    ~LineSegment();

public:
    int getMean();
    int getStdDev();

public:
    float meanval_adjangles, stddev_adjangles; //
};

template< unsigned int VImageDimension >
int runGPUGradientAnisotropicDiffusionImageFilter(const std::string& inFile, const std::string& outFile)
{
    if(!itk::IsGPUAvailable())
    {
        std::cerr << "OpenCL-enabled GPU is not present." << std::endl;
        return EXIT_FAILURE;
    }

    qDebug()<<"test ... ";

    typedef float InputPixelType;
    typedef float OutputPixelType;

//    typedef itk::GPUImage< InputPixelType,  VImageDimension >   InputImageType;
//    typedef itk::GPUImage< OutputPixelType, VImageDimension >   OutputImageType;

    typedef itk::Image< InputPixelType,  VImageDimension >   InputImageType;
    typedef itk::Image< OutputPixelType, VImageDimension >   OutputImageType;

    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    typedef itk::ImageFileWriter< OutputImageType >  WriterType;

    typename ReaderType::Pointer reader = ReaderType::New();
    typename WriterType::Pointer writer = WriterType::New();

    qDebug()<<"before reading ...";

    reader->SetFileName( inFile );
    writer->SetFileName( outFile );

    qDebug()<<"set input and output ... ";

    // Create CPU/GPU anistropic diffusion filter
    typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > CPUAnisoDiffFilterType;
    typename CPUAnisoDiffFilterType::Pointer CPUFilter = CPUAnisoDiffFilterType::New();

    qDebug()<<"init cpu anisoDiffFilter ...";

    //typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > GPUAnisoDiffFilterType;
    //typename GPUAnisoDiffFilterType::Pointer GPUFilter = GPUAnisoDiffFilterType::New();

    //
    reader->Update();

    qDebug()<<"data loaded ...";

    // CPU anistropic diffusion
    itk::TimeProbe cputimer;
    cputimer.Start();

    CPUFilter->SetNumberOfThreads( 8 );

    CPUFilter->SetInput( reader->GetOutput() );
    CPUFilter->SetNumberOfIterations( 10 );
    CPUFilter->SetTimeStep( 0.0625 );
    CPUFilter->SetConductanceParameter( 3.0 );
    CPUFilter->UseImageSpacingOn();
    CPUFilter->Update();

    cputimer.Stop();

    std::cout << "CPU Anisotropic diffusion took " << cputimer.GetMean() << " seconds with "
              << CPUFilter->GetNumberOfThreads() << " threads.\n" << std::endl;

    writer->SetInput( CPUFilter->GetOutput() );
    writer->Update();


    // GPU anistropic diffusion
//    itk::TimeProbe gputimer;
//    gputimer.Start();

//    GPUFilter->SetInput( reader->GetOutput() );
//    GPUFilter->SetNumberOfIterations( 10 );
//    GPUFilter->SetTimeStep( 0.0625 );
//    GPUFilter->SetConductanceParameter( 3.0 );
//    GPUFilter->UseImageSpacingOn();

//    try
//    {
//        GPUFilter->Update();
//    }
//    catch (itk::ExceptionObject& excp)
//    {
//        std::cout << "Caught exception during GPUFilter->Update() " << excp << std::endl;
//        return EXIT_FAILURE;
//    }

//    try
//    {
//        GPUFilter->GetOutput()->UpdateBuffers(); // synchronization point
//    }
//    catch (itk::ExceptionObject& excp)
//    {
//        std::cout << "Caught exception during GPUFilter->GetOutput()->UpdateBuffers() " << excp << std::endl;
//        return EXIT_FAILURE;
//    }

//    //
//    gputimer.Stop();
//    std::cout << "GPU Anisotropic diffusion took " << gputimer.GetMean() << " seconds.\n" << std::endl;

//    writer->SetInput( GPUFilter->GetOutput() ); // copy GPU->CPU implicilty

//    // execute pipeline filter and write output
//    writer->Update();

//    GPUFilter = ITK_NULLPTR; // explicit GPU object destruction test
//    itk::GPUContextManager::GetInstance()->DestroyInstance(); // GPUContextManager singleton destruction test

    //
    return EXIT_SUCCESS;
}

#endif // _NEURONRECON_H_
