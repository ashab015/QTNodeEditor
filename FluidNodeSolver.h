#include <BulletFluids\btFluidRigidDynamicsWorld.h>
#include <BulletFluids\Sph\btFluidSph.h>
#include <BulletFluids\Sph\btFluidSortingGrid.h>
#include <BulletFluids\Sph\btFluidSphSolver.h>
#include <iostream>
#include <ios>
#include <string>
#include "FluidHelpers.h"
#include "FluidNodeEditor.h"
#include "VectorGrid.h"

class ViscoelasticSolver;

static btVector3 BTP(Point3 pos)
{
	return btVector3(pos.x, pos.z, pos.y);
}

static Point3 PTB(btVector3 vec)
{
	return Point3(vec.x(), vec.z(), vec.y());
}

class FluidNode
{

public:
	FluidNode(INode* n, FunctionNodes nodeint)
	{
		node = n;
		nodetype = nodeint;
		extravalue = 0;
		extravalue2 = 0;

		if (nodeint == Collider)
		{
			Functions func;
			Mesh mesh = *func.getMesh(n);
			TriObject* p_triobj = func.getTriObj(node);
			btTriangleMesh* meshInterface = new btTriangleMesh();	
			Matrix3 objectTM = node->GetObjectTM(GetCOREInterface()->GetTime());
			for (int i=0;i<mesh.getNumFaces();i++)
			{
				Point3 p0 = mesh.getVert(mesh.faces[i].v[0]);
				Point3 p1 = mesh.getVert(mesh.faces[i].v[1]);
				Point3 p2 = mesh.getVert(mesh.faces[i].v[2]);							
				meshInterface->addTriangle( btVector3(p0.x,p0.z,p0.y),btVector3(p1.x,p1.z,p1.y),btVector3(p2.x,p2.z,p2.y));
			}
			btBvhTriangleMeshShape* trimesh = new btBvhTriangleMeshShape(meshInterface,true);
			btTransform trans = func.Matrix3toTransform(node, false);
			collider = CreateRigidBody(0.0f, trans, trimesh, false);
			collider->setCollisionFlags(collider->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);   
			collider->setActivationState(DISABLE_DEACTIVATION);
		}
	}
	FluidNode(FunctionNodes nodeint)
	{
		nodetype = nodeint;
		extravalue = 0;
		extravalue2 = 0;
	}
	INode* node;
	float extravalue;
	float extravalue2;
	float extravalue3;
	FunctionNodes nodetype;
	btRigidBody* collider;

};

struct NodeContactResult : public btCollisionWorld::ContactResultCallback
{
	bool m_connected;
	btScalar m_margin;
	NodeContactResult() :m_connected(false), m_margin(0.05)
	{
	}
	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		if (cp.getDistance() <= m_margin)
			m_connected = true;
		return 1.f;
	}
};

class FluidNodeSolver
{

public:
	FluidNodeSolver(INode* node, btDynamicsWorld* btworld, btFluidSph* sph) 
	{
		particlenode = node;
		world = btworld;
		fluid = sph;
	}
	~FluidNodeSolver()
	{
		includednodes.clear();
		nodes.clear();
		links.clear();
		fluidnodes.clear();
	}
	void Init()
	{

		// init velgrid
		btVector3 bbox = fluid->getLocalParameters().m_aabbBoundaryMax;
		vgrid = new VectorGrid(8, bbox);

		LoadNodeLogic();
		SolveNodeLogic();
	}
	void StepSimulation();
	void LoadNodeLogic();
	void SolveNodeLogic();
	void SolveCrowdNodes();
	void SolveVectorGrid();
	void SetVSolver(ViscoelasticSolver* solver)
	{
		vsolver = solver;
	}
	void SetNeededNodes(std::vector<INode*> nodes)
	{
		includednodes = nodes;
	}
	void SetNeededCurves(std::vector<INode*> curves)
	{
		curvenodes = curves;
	}
	VectorGrid* GetVectorGrid()
	{
		return vgrid;
	}

private:
	int startframe;
	int endframe;
	INode* particlenode;

public:
	btFluidSph* fluid;

private:
	std::vector<INode*> includednodes;
	std::vector<INode*> curvenodes;
	std::vector<QTNode*> nodes;
	std::vector<QTNodeLink*> links;
	std::vector<FluidNode*> fluidnodes;
	std::vector<btRigidBody*> colliders;
	btDynamicsWorld* world;
	ViscoelasticSolver* vsolver;
	VectorGrid* vgrid;
	btFluidSphParametersGlobal globalparams;

private:
	bool PointInAABB(const btVector3& volMin, const btVector3& volMax, const btVector3& vecPoint)
	{
		//Check if the point is less than max and greater than min
		if (vecPoint.x() > volMin.x() && vecPoint.x() < volMax.x() &&
			vecPoint.y() > volMin.y() && vecPoint.y() < volMax.y() &&
			vecPoint.z() > volMin.z() && vecPoint.z() < volMax.z())
		{
			return true;
		}
		//If not, then return false
		return false;
	}

};