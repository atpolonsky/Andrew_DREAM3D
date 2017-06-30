/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
* Copyright (c) 2014 The Regents of the University of California, Author: William Lenthe
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "ErodeDilateBadData.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "Processing/ProcessingConstants.h"

// Include the MOC generated file for this class
#include "moc_ErodeDilateBadData.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateBadData::ErodeDilateBadData() :
  AbstractFilter(),
  m_Direction(0),
  m_NumIterations(1),
  m_XDirOn(true),
  m_YDirOn(true),
  m_ZDirOn(true),
  m_ReplaceBadData(true),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_Neighbors(NULL),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ErodeDilateBadData::~ErodeDilateBadData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operation");
    parameter->setPropertyName("Direction");

    QVector<QString> choices;
    choices.push_back("Dilate");
    choices.push_back("Erode");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(IntFilterParameter::New("Number of Iterations", "NumIterations", getNumIterations(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("X Direction", "XDirOn", getXDirOn(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("Y Direction", "YDirOn", getYDirOn(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("Z Direction", "ZDirOn", getZDirOn(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("Replace Bad Data", "ReplaceBadData", getReplaceBadData(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setDirection( reader->readValue("Direction", getDirection()) );
  setNumIterations( reader->readValue("NumIterations", getNumIterations()) );
  setXDirOn(reader->readValue("XDirOn", getXDirOn()) );
  setYDirOn(reader->readValue("YDirOn", getYDirOn()) );
  setZDirOn(reader->readValue("ZDirOn", getZDirOn()) );
  setReplaceBadData(reader->readValue("ReplaceBadData", getReplaceBadData()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ErodeDilateBadData::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(Direction)
  SIMPL_FILTER_WRITE_PARAMETER(NumIterations)
  SIMPL_FILTER_WRITE_PARAMETER(XDirOn)
  SIMPL_FILTER_WRITE_PARAMETER(YDirOn)
  SIMPL_FILTER_WRITE_PARAMETER(ZDirOn)
  SIMPL_FILTER_WRITE_PARAMETER(ReplaceBadData)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  if (getNumIterations() <= 0)
  {
    QString ss = QObject::tr("The number of iterations (%1) must be positive").arg(getNumIterations());
    setErrorCondition(-5555);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ErodeDilateBadData::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValue(-1);

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  int32_t good = 1;
  int64_t count = 0;
  int64_t kstride = 0, jstride = 0;
  int32_t featurename = 0, feature = 0;
  int32_t current = 0;
  int32_t most = 0;
  int64_t neighpoint = 0;
  size_t numfeatures = 0;

  for (size_t i = 0; i < totalPoints; i++)
  {
    featurename = m_FeatureIds[i];
    if (featurename > numfeatures) { numfeatures = featurename; }
  }

  DimType neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];

  QVector<int32_t> n(numfeatures + 1, 0);

  for (int32_t iteration = 0; iteration < m_NumIterations; iteration++)
  {
    for (DimType k = 0; k < dims[2]; k++)
    {
      kstride = dims[0] * dims[1] * k;
      for (DimType j = 0; j < dims[1]; j++)
      {
        jstride = dims[0] * j;
        for (DimType i = 0; i < dims[0]; i++)
        {
          count = kstride + jstride + i;
          featurename = m_FeatureIds[count];
          if (featurename == 0)
          {
            current = 0;
            most = 0;
            for (int32_t l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = count + neighpoints[l];
              if (l == 0 && (k == 0 || m_ZDirOn == false)) { good = 0; }
              else if (l == 5 && (k == (dims[2] - 1) || m_ZDirOn == false)) { good = 0; }
              else if (l == 1 && (j == 0 || m_YDirOn == false)) { good = 0; }
              else if (l == 4 && (j == (dims[1] - 1) || m_YDirOn == false)) { good = 0; }
              else if (l == 2 && (i == 0 || m_XDirOn == false)) { good = 0; }
              else if (l == 3 && (i == (dims[0] - 1) || m_XDirOn == false)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if (m_Direction == 0 && feature > 0)
                {
                  m_Neighbors[neighpoint] = count;
                }
                if (feature > 0 && m_Direction == 1)
                {
                  n[feature]++;
                  current = n[feature];
                  if (current > most)
                  {
                    most = current;
                    m_Neighbors[count] = neighpoint;
                  }
                }
              }
            }
            if (m_Direction == 1)
            {
              for (int32_t l = 0; l < 6; l++)
              {
                good = 1;
                neighpoint = count + neighpoints[l];
                if (l == 0 && k == 0) { good = 0; }
                if (l == 5 && k == (dims[2] - 1)) { good = 0; }
                if (l == 1 && j == 0) { good = 0; }
                if (l == 4 && j == (dims[1] - 1)) { good = 0; }
                if (l == 2 && i == 0) { good = 0; }
                if (l == 3 && i == (dims[0] - 1)) { good = 0; }
                if (good == 1)
                {
                  feature = m_FeatureIds[neighpoint];
                  n[feature] = 0;
                }
              }
            }
          }
        }
      }
    }

    QString attrMatName = m_FeatureIdsArrayPath.getAttributeMatrixName();
    QList<QString> voxelArrayNames = m->getAttributeMatrix(attrMatName)->getAttributeArrayNames();

    for (size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      int32_t neighbor = m_Neighbors[j];
      if (neighbor >= 0)
      {
        if ( (featurename == 0 && m_FeatureIds[neighbor] > 0 && m_Direction == 1)
             || (featurename > 0 && m_FeatureIds[neighbor] == 0 && m_Direction == 0))
        {
          if (getReplaceBadData())
          {
            for(QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
            {
              IDataArray::Pointer p = m->getAttributeMatrix(attrMatName)->getAttributeArray(*iter);
              p->copyTuple(neighbor, j);
            }
          }
          else
          {
            m_FeatureIds[j] = m_FeatureIds[neighbor];
          }
        }
      }
    }
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ErodeDilateBadData::newFilterInstance(bool copyFilterParameters)
{
  ErodeDilateBadData::Pointer filter = ErodeDilateBadData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateBadData::getCompiledLibraryName()
{ return ProcessingConstants::ProcessingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateBadData::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateBadData::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ErodeDilateBadData::getHumanLabel()
{ return "Erode/Dilate Bad Data"; }