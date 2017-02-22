#include "Mesh_FaceTopology.h"


#include "TriMesh.h"
#include "MeshTopologyQueries.h"


using namespace Urho3D;

String Mesh_FaceTopology::iconTexture = "Textures/Icons/Mesh_FaceTopology.png";

Mesh_FaceTopology::Mesh_FaceTopology(Context* context) : IoComponentBase(context, 1, 3)
{
    SetName("FaceTopology");
    SetFullName("Compute Mesh Face Topology");
    SetDescription("Computes Mesh Topology Data for a given face");
    SetGroup(IoComponentGroup::MESH);
    SetSubgroup("Analysis");
    
    inputSlots_[0]->SetName("Mesh");
    inputSlots_[0]->SetVariableName("M");
    inputSlots_[0]->SetDescription("TriMeshWithAdjacencyData");
    inputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
    inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
    
    //inputSlots_[1]->SetName("FaceID");
    //inputSlots_[1]->SetVariableName("ID");
    //inputSlots_[1]->SetDescription("ID of face of interest");
    //inputSlots_[1]->SetVariantType(VariantType::VAR_INT);
    //inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
    //inputSlots_[1]->SetDefaultValue(0);
    //inputSlots_[1]->DefaultSet();
    
    outputSlots_[0]->SetName("IncidentVertIDs");
    outputSlots_[0]->SetVariableName("V_ID");
    outputSlots_[0]->SetDescription("Indices of vertices incident to F");
    outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[0]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[1]->SetName("IncidentVertices");
    outputSlots_[1]->SetVariableName("V");
    outputSlots_[1]->SetDescription("Vertices incident to F");
    outputSlots_[1]->SetVariantType(VariantType::VAR_VECTOR3);
    outputSlots_[1]->SetDataAccess(DataAccess::LIST);
    
    outputSlots_[2]->SetName("AdjFaces");
    outputSlots_[2]->SetVariableName("F");
    outputSlots_[2]->SetDescription("Indices of faces adjacent to F");
    outputSlots_[2]->SetVariantType(VariantType::VAR_INT);
    outputSlots_[2]->SetDataAccess(DataAccess::LIST);
    
}

//void Mesh_FaceTopology::SolveInstance(
//                                        const Vector<Variant>& inSolveInstance,
//                                        Vector<Variant>& outSolveInstance
//                                        )
//{
//    
//    ///////////////////
//    // VERIFY & EXTRACT
//    
//    // Verify input slot 0
//    Variant inMesh = inSolveInstance[0];
//    if (!TriMesh_HasAdjacencyData(inMesh)) {
//        URHO3D_LOGWARNING("M must be a valid mesh with adjacency data.");
//        outSolveInstance[0] = Variant();
//        return;
//    }
//    
//    // Verify input slot 1
//    VariantType type = inSolveInstance[1].GetType();
//    if (!(type == VariantType::VAR_INT)) {
//        URHO3D_LOGWARNING("ID must be a valid integer.");
//        outSolveInstance[0] = Variant();
//        return;
//    }
//    
//    int faceID = inSolveInstance[1].GetInt();
//    VariantMap meshWithData = inMesh.GetVariantMap();
//    VariantMap triMesh = meshWithData["mesh"].GetVariantMap();
//    
//    Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(triMesh));
//	Urho3D::VariantVector normalList = TriMesh_GetNormalList(Variant(triMesh));
//
//    //check that faceID is within range
//	if (faceID < 0 || faceID > normalList.Size() - 1) {
//		URHO3D_LOGWARNING("FaceID is out of range");
//		SetAllOutputsNull(outSolveInstance);
//		return;
//	}
//    
//    
//    ///////////////////
//    // COMPONENT'S WORK
//    
//    VariantVector vert_ids = TriMesh_FaceToVertices(inMesh, faceID);
//    VariantVector adj_faces = TriMesh_FaceToFaces(inMesh, faceID);
//    VariantVector vertexVectors;
//    for (int i = 0; i < vert_ids.Size(); ++i)
//    {
//        int v_ID = vert_ids[i].GetInt();
//        Vector3 curVert = vertexList[v_ID].GetVector3();
//        vertexVectors.Push(Variant(curVert));
//    }
//    
//    
//    /////////////////
//    // ASSIGN OUTPUTS
//    
//    outSolveInstance[0] = vert_ids;
//    outSolveInstance[1] = vertexVectors;
//    outSolveInstance[2] = adj_faces;
//    
//}

int Mesh_FaceTopology::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 3);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	IoDataTree inputMeshTree = inputSlots_[0]->GetIoDataTree();

	Variant inMesh;
	Urho3D::Vector<int> path;
	path.Push(0);

	// for some reason the component was crashing on initialization without this:
	inputMeshTree.GetItem(inMesh, path, 0);
	if (inMesh.GetType() == VAR_NONE)
	{
		solvedFlag_ = 0;
		return 0;
	}


	if (!TriMesh_HasAdjacencyData(inMesh)) {
		URHO3D_LOGWARNING("M must be a TriMesh WITH DATA (use Mesh_ComputeAdjacencyData)!");
		solvedFlag_ = 0;
		return 0;
	}

	IoDataTree face_vertex_vectors(GetContext());
	IoDataTree face_vertex_IDs = ComputeVerticesOnFace(inMesh, face_vertex_vectors);
	IoDataTree adjacent_faces = ComputeAdjacentFaces(inMesh);

	outputSlots_[0]->SetIoDataTree(face_vertex_IDs);
	outputSlots_[1]->SetIoDataTree(face_vertex_vectors);
	outputSlots_[2]->SetIoDataTree(adjacent_faces);

	solvedFlag_ = 1;
	return 1;
}

IoDataTree Mesh_FaceTopology::ComputeVerticesOnFace(Urho3D::Variant inMeshWithData, IoDataTree& vertexVectorsTree)
{
	IoDataTree face_vertex_tree(GetContext());

	VariantMap* meshWithData = inMeshWithData.GetVariantMapPtr();
	VariantMap* triMesh = (*meshWithData)["mesh"].GetVariantMapPtr();

	Urho3D::VariantVector vertexList = TriMesh_GetVertexList(Variant(*triMesh));
	VariantVector face_list = TriMesh_GetFaceList(Variant(*triMesh));

	int num = face_list.Size() / 3;

	for (int i = 0; i < num; ++i) {
		Vector<int> path;
		path.Push(i);

		VariantVector vertex_IDs; 
		vertex_IDs.Push(face_list[3 * i].GetInt());
		vertex_IDs.Push(face_list[3 * i + 1].GetInt());
		vertex_IDs.Push(face_list[3 * i + 2].GetInt());

		face_vertex_tree.Add(path, vertex_IDs);

		// compute the vectors of these verts at the same time.
		VariantVector vertex_vectors;
		for (int j = 0; j < vertex_IDs.Size(); ++j) {
			int curVert = vertex_IDs[j].GetInt();
			vertex_vectors.Push(Variant(vertexList[curVert].GetVector3()));
		}
		vertexVectorsTree.Add(path, vertex_vectors);
	}

	return face_vertex_tree;
}

IoDataTree Mesh_FaceTopology::ComputeAdjacentFaces(Urho3D::Variant inMeshWithData)
{
	IoDataTree adjacent_faces_tree(GetContext());

	VariantMap* meshWithData = inMeshWithData.GetVariantMapPtr();
	VariantMap* triMesh = (*meshWithData)["mesh"].GetVariantMapPtr();

	VariantVector* face_face_list = (*meshWithData)["face-face"].GetVariantVectorPtr();

	for (int i = 0; i < (*face_face_list).Size(); ++i) {
		Vector<int> path;
		path.Push(i);

		VariantVector adj_faces = (*face_face_list)[i].GetVariantVector();
		adjacent_faces_tree.Add(path, adj_faces);

	}

	return adjacent_faces_tree;
}