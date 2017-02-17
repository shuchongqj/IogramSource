#include "Mesh_FlipNormals.h"

#include <Urho3D/Core/Variant.h>

#include "TriMesh.h"

using namespace Urho3D;

String Mesh_FlipNormals::iconTexture = "";

Mesh_FlipNormals::Mesh_FlipNormals(Context* context) : IoComponentBase(context, 1, 1)
{
    SetName("FlipNormals");
    SetFullName("Unify Normals");
    SetDescription("Unify TriMesh normals to consistent orientation");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Operators");
    
    inputSlots_[0]->SetName("MeshIn");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("Mesh in");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    outputSlots_[0]->SetName("MeshOut");
    outputSlots_[0]->SetVariableName("N");
    outputSlots_[0]->SetDescription("Mesh with flipped normals");
    outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_FlipNormals::SolveInstance(
                                      const Vector<Variant>& inSolveInstance,
                                      Vector<Variant>& outSolveInstance
                                      )
{
    Variant mesh_in = inSolveInstance[0];
    if (!TriMesh_Verify(mesh_in)) {
        URHO3D_LOGWARNING("M must be a TriMesh!");
        SetAllOutputsNull(outSolveInstance);
        return;
    }
    
    Variant mesh_out = TriMesh_FlipNormals(mesh_in);
    
    outSolveInstance[0] = mesh_out;
}