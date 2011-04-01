#include "v3dView4DInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>

#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkCollection.h>
#include <vtkMetaDataSetSequence.h>

#include "v3dView.h"
#include <QInputDialog>
#include <QColorDialog>

class v3dView4DInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    dtkAbstractData  *data;
    v3dView          *view;
    vtkCollection* sequenceList;
};

v3dView4DInteractor::v3dView4DInteractor(): med4DAbstractViewInteractor(), d(new v3dView4DInteractorPrivate)
{
    d->data = NULL;
    d->view = NULL;
    this->currentTime = 0.0;
    d->sequenceList = vtkCollection::New();
}

v3dView4DInteractor::~v3dView4DInteractor()
{
  this->disable();
  d->sequenceList->Delete();
  
  delete d;
  d = 0;
}

QString v3dView4DInteractor::description(void) const
{
  return "v3dView4DInteractor";
}

QStringList v3dView4DInteractor::handled(void) const
{
  return QStringList () << "v3dView";
}

bool v3dView4DInteractor::registered(void)
{
  return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dView4DInteractor", QStringList() << "v3dView", createV3dView4DInteractor);
}

void v3dView4DInteractor::setData(dtkAbstractData *data)
{
  //  this->appendData (data);
}

void v3dView4DInteractor::appendData(dtkAbstractData *data)
{
  if (vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data())))
  {
    d->sequenceList->AddItem (sequence);
    this->updatePipeline ();
  }
}

void v3dView4DInteractor::setView(dtkAbstractView *view)
{
  if (v3dView *v3dview = dynamic_cast<v3dView*>(view) )
  {
    d->view = v3dview;
    connect (view, SIGNAL (dataAdded (dtkAbstractData*)), this, SLOT (onDataAdded (dtkAbstractData*)));
  }

}

void v3dView4DInteractor::onDataAdded(dtkAbstractData *data)
{
  if ( data->description() == "vtkDataMesh4D" )
  {  
    this->appendData(data);
  }
  else
  {
    if ( data->description().contains ("4") )
    {
      itk::Image<short, 4>* image = dynamic_cast<itk::Image<short, 4>*>( (itk::Object*)( data->data() ) );
      
      if (image)
      {
	vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();    
	sequence->SetITKDataSet<short> (image);
	d->sequenceList->AddItem (sequence);
	sequence->Delete();
      }
      
      this->updatePipeline();
    }
  }
}

bool v3dView4DInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
  if ( data->description().contains ("4") ) {
    this->enable ();
    return true;
  }

  return false;
}

void v3dView4DInteractor::enable(void)
{
  if (this->enabled())
    return;
  this->updatePipeline ();
  qDebug()<<" v3dView4DInteractor::enable";
  med4DAbstractViewInteractor::enable();
}


void v3dView4DInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view)
    {
      for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
      {
	vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject(i));
	if (!sequence)
	  continue;
	
	// d->view->view2d ()->RemoveDataset (sequence->GetDataSet());
	// d->view->view3d ()->RemoveDataset (sequence->GetDataSet());
      }
    }
    med4DAbstractViewInteractor::disable();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dView4DInteractor(void)
{
  return new v3dView4DInteractor;
}

void v3dView4DInteractor::updatePipeline (void)
{
  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence *sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;

    switch (sequence->GetType())
    {
	case vtkMetaDataSet::VTK_META_IMAGE_DATA:
	  d->view->view2d()->SetInput (vtkImageData::SafeDownCast (sequence->GetDataSet()));
	  d->view->view3d()->SetInput (vtkImageData::SafeDownCast (sequence->GetDataSet()));
	  break;
	case vtkMetaDataSet::VTK_META_SURFACE_MESH:
	case vtkMetaDataSet::VTK_META_VOLUME_MESH:
	  d->view->view2d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));
	  d->view->view3d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()));
	  break;  
	default:
	  break;
    }
  }
}

void v3dView4DInteractor::setCurrentTime (double time)
{
  if (!d->view)
    return;
  
  double range[2] = {0,0};
  this->sequencesRange(range);

  time = std::min (range[1], time);
  time = std::max (range[0], time);

  this->currentTime = time;

  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    sequence->UpdateToTime (time);
  }

  d->view->currentView()->Modified();
  d->view->currentView()->Render();
}


void v3dView4DInteractor::sequencesRange (double* range)
{
  if (!d->sequenceList->GetNumberOfItems())
  {
    range[0] = 0;
    range[1] = 1.0;
    return;
  }
  
  double mintime = 3000;
  double maxtime = -3000;
  
  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    mintime = std::min (mintime, sequence->GetMinTime());
    maxtime = std::max (maxtime, sequence->GetMaxTime());
  }

  range[0] = mintime;
  range[1] = maxtime;
}


double v3dView4DInteractor::sequencesMinTimeStep (void)
{
  if (!d->sequenceList->GetNumberOfItems())
  {
    return 0.01;
  }
  
  double step = 3000;
  
  for (int i=0; i<d->sequenceList->GetNumberOfItems(); i++)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast(d->sequenceList->GetItemAsObject (i));
    if (!sequence)
      continue;
    double mintime = sequence->GetMinTime();
    double maxtime = sequence->GetMaxTime();
    double number = sequence->GetNumberOfMetaDataSets();

    step = std::min ( step, (maxtime - mintime)/(number - 1.0) );  
  }  

  return step;
}

