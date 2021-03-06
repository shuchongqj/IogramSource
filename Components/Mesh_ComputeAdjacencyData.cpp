//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Mesh_ComputeAdjacencyData.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_ComputeAdjacencyData::iconTexture = "Textures/Icons/Mesh_ComputeAdjacencyData.png";

Mesh_ComputeAdjacencyData::Mesh_ComputeAdjacencyData(Context* context) : IoComponentBase(context, 1, 1)
{
    SetName("ComputeAdjacencyData");
    SetFullName("Compute Mesh Topology Data");
    SetDescription("Computes Mesh Topology Data (manifold trimeshes only!)");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Operators");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMesh");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[0]->SetName("Mesh");
    outputSlots_[0]->SetVariableName("M");
    outputSlots_[0]->SetDescription("Mesh with topology data computed");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
}

void Mesh_ComputeAdjacencyData::SolveInstance(
                                   const Vector<Variant>& inSolveInstance,
                                   Vector<Variant>& outSolveInstance
                                   )
{
    
    ///////////////////
    // VERIFY & EXTRACT
    
    // Verify input slot 0
    Variant inMesh = inSolveInstance[0];
    if (!TriMesh_Verify(inMesh)) {
        URHO3D_LOGWARNING("M must be a valid mesh.");
        outSolveInstance[0] = Variant();
        return;
    }
    
    ///////////////////
    // COMPONENT'S WORK
    
    Variant out_mesh = TriMesh_ComputeAdjacencyData(inMesh);
    
    /////////////////
    // ASSIGN OUTPUTS
    
    outSolveInstance[0] = out_mesh;

}
