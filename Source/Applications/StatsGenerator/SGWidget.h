/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _SGWIDGET_H_
#define _SGWIDGET_H_

#include <QtWidgets/QWidget>


#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class SGWidget : public QWidget
{

    Q_OBJECT

  public:
    SGWidget(QWidget* parent = 0);
    virtual ~SGWidget();

    SIMPL_VIRTUAL_INSTANCE_PROPERTY(unsigned int, PhaseType)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(int, PhaseIndex)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
    SIMPL_VIRTUAL_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

    virtual void extractStatsData(AttributeMatrix::Pointer attrMat, int index);
    virtual int gatherStatsData(AttributeMatrix::Pointer attrMat);

    virtual QString getComboString();
    virtual QString getTabTitle();

    virtual void on_m_GenerateDefaultData_clicked();

  public slots:


  protected slots:

    virtual void dataWasEdited();

  protected:


  private:


    SGWidget(const SGWidget&); // Copy Constructor Not Implemented
    void operator=(const SGWidget&); // Operator '=' Not Implemented

};

#endif /* SGWIDGET_H_ */

