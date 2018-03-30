#include "FluidNodeSolver.h"
#include "Viscoelastic.h"

/// <summary> Translates the users node setup to the 3dsmax scene. </summary>
void FluidNodeSolver::LoadNodeLogic()
{
	using namespace std;
	std::string path = Get3dsmaxPath2() + TCHARToString2(particlenode->GetName()) + ".cfx";
	string line;
  	ifstream myfile(path);
	std::vector<int> indexs;
	std::vector<QVector2D> postions;
	std::vector<IntPair> linknodes;
	std::vector<IntPair> linkindex;
	std::vector<IntPair> linkout;
	std::vector<bool> intbool;
	std::vector<float> spinnerindex;

  	if (myfile.is_open())
  	{
    	while (getline (myfile,line))
    	{
      	    if (line[0] == 'n' && line[1] == 'i')
			{
				line[0] = ' '; 
				line[1] = ' ';
				line.erase(remove(line.begin(), line.end(), ' '), line.end());
				int index = atoi(line.c_str());
				indexs.push_back(index);
				intbool.push_back(false);
				spinnerindex.push_back(0);
			}
			if (line[0] == 'l' && line[1] == 'n')
			{
				line[0] = ' '; 
				line[1] = ' ';
				std::string str = line;
		     	std::istringstream buf(str);
		   	 	std::istream_iterator<std::string> beg(buf), end;
		   	 	std::vector<std::string> tokens(beg, end); // done!
		   	 	float px = std::atof(tokens[0].c_str());
		  	    float py = std::atof(tokens[1].c_str());
				linknodes.push_back(IntPair(px, py));
			}
			if (line[0] == 'l' && line[1] == 'i')
			{
				line[0] = ' '; 
				line[1] = ' ';
				std::string str = line;
		     	std::istringstream buf(str);
		   	 	std::istream_iterator<std::string> beg(buf), end;
		   	 	std::vector<std::string> tokens(beg, end); // done!
		   	 	float px = std::atof(tokens[0].c_str());
		  	    float py = std::atof(tokens[1].c_str());
				linkindex.push_back(IntPair(px, py));
			}
			if (line[0] == 'l' && line[1] == 'o')
			{
				line[0] = ' '; 
				line[1] = ' ';
				std::string str = line;
		     	std::istringstream buf(str);
		   	 	std::istream_iterator<std::string> beg(buf), end;
		   	 	std::vector<std::string> tokens(beg, end); // done!
		   	 	float px = std::atof(tokens[0].c_str());
		  	    float py = std::atof(tokens[1].c_str());
				linkout.push_back(IntPair(px, py));
			}
			if (line[0] == 'e' && line[1] == 'd')
			{
				line[0] = ' '; 
				line[1] = ' ';
				std::string str = line;
		     	std::istringstream buf(str);
		   	 	std::istream_iterator<std::string> beg(buf), end;
		   	 	std::vector<std::string> tokens(beg, end); // done!
		   	 	float px = std::atoi(tokens[0].c_str());
		  	    float py = std::atoi(tokens[1].c_str());
				intbool[px] = true;
				spinnerindex[px] = py;
			}
    	}
  	}

	for (int x=0; indexs.size() > x; x++)
	{
		QTNode* node = new QTNode();
		node->SetNode(indexs[x]);
		if (intbool[x] == true)
		{
			node->spinnervalue = spinnerindex[x];
		}
		nodes.push_back(node);
	}

	for (int x=0; linkout.size() > x; x++)
	{
		QTNodeLink* link = new QTNodeLink();
		link->node1 = nodes[linknodes[x].i1];
		link->node2 = nodes[linknodes[x].i2];
		link->nodeindex1 = linknodes[x].i1;
		link->nodeindex2 = linknodes[x].i2;
		link->inputoroutput1 = linkout[x].i1;
		link->inputoroutput2 = linkout[x].i2;
		link->inputindex1 = linkindex[x].i1;
		link->inputindex2 = linkindex[x].i2;
		links.push_back(link);
	}

	// Solve logic
	SolveNodeLogic();
}

void FluidNodeSolver::SolveNodeLogic()
{
	for (int x=0; nodes.size() > x; x++)
	{
		// Node Repel
		if (nodes[x]->name == "Node Repel")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			float extraval = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && includednodes.size() > 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;
						continue;					
					}
					if (nodes[nodeinput]->name == "Int" || nodes[nodeinput]->name == "Float" && iinput1 == 1 && includednodes.size() > 0)
					{
						extraval = nodes[nodeinput]->spinnervalue;			
						continue;
					}
				}
			}
			if (includednodes.size() > spinnervalue && spinnervalue >= 0 && extraval != -1)
			{
				FluidNode* node = new FluidNode(includednodes[spinnervalue], NodeRepel);
				fluidnodes.push_back(node);
			}
		}

		// Node Attract
		if (nodes[x]->name == "Node Attract")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			float extraval = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && includednodes.size() > 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;
						continue;					
					}
					if (nodes[nodeinput]->name == "Int" || nodes[nodeinput]->name == "Float" && iinput1 == 1 && includednodes.size() > 0)
					{
						extraval = nodes[nodeinput]->spinnervalue;			
						continue;
					}
				}
			}
			if (includednodes.size() > spinnervalue && spinnervalue >= 0 && extraval != -1)
			{
				FluidNode* node = new FluidNode(includednodes[spinnervalue], NodeAttract);
				fluidnodes.push_back(node);
			}
		}
		
		// Node Colliders
		if (nodes[x]->name == "Collider")
		{
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					int iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					int nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && includednodes.size() > 0)
					{
						int spinnervalue = nodes[nodeinput]->spinnervalue;
						if (includednodes.size() > spinnervalue && spinnervalue >= 0)
						{
							FluidNode* node = new FluidNode(includednodes[spinnervalue], Collider);
							fluidnodes.push_back(node);
						}			
					}
				}
			}
		}

		// Path
		if (nodes[x]->name == "Path")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			float extraval = -1;
			float extraval2 = -1;
			float extraval3 = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && curvenodes.size() > 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;
						continue;					
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 1 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 1 && curvenodes.size() > 0)
					{
						extraval = nodes[nodeinput]->spinnervalue;			
						continue;
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 2 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 2 && curvenodes.size() > 0)
					{
						extraval2 = nodes[nodeinput]->spinnervalue;			
						continue;
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 3 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 3 && curvenodes.size() > 0)
					{
						extraval3 = nodes[nodeinput]->spinnervalue;			
						continue;
					}
				}
			}
			if (curvenodes.size() > spinnervalue && spinnervalue >= 0 && extraval != -1 && extraval2 != -1 && extraval3 != -1)
			{
				FluidNode* node = new FluidNode(curvenodes[spinnervalue], Path);
				node->extravalue = extraval;
				node->extravalue2 = extraval2;
				node->extravalue3 = extraval3;
				fluidnodes.push_back(node);
			}
		}

		// Node Morph
		if (nodes[x]->name == "Morpher")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			float extraval = -1;
			float extraval2 = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && includednodes.size() > 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;
						continue;					
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 1 && includednodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 1 && includednodes.size() > 0)
					{
						extraval = nodes[nodeinput]->spinnervalue;			
						continue;
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 2 && includednodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 2 && includednodes.size() > 0)
					{
						extraval2 = nodes[nodeinput]->spinnervalue;			
						continue;
					}
				}
			}
			if (includednodes.size() > spinnervalue && spinnervalue >= 0 && extraval != -1 && extraval2 != -1)
			{
				FluidNode* node = new FluidNode(includednodes[spinnervalue], Morpher);
				node->extravalue = extraval;
				node->extravalue2 = extraval2;
				fluidnodes.push_back(node);
			}
		}

		// gravity
		if (nodes[x]->name == "Gravity")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;		
					}
				}
			}
		}


		// Node Morph
		if (nodes[x]->name == "Path Morpher")
		{
			int iinput = -1;
			int nodeinput = -1;
			int spinnervalue = -1;
			float extraval = -1;
			float extraval2 = -1;
			float extraval3 = -1;
			for (int i=0; links.size() > i; i++)
			{
				if (links[i]->nodeindex1 == x)
				{
					iinput = links[i]->inputindex2;
					int iinput1 = links[i]->inputindex1;
					nodeinput = links[i]->nodeindex2;
					if (nodes[nodeinput]->name == "Int" && iinput1 == 0 && curvenodes.size() > 0)
					{
						spinnervalue = nodes[nodeinput]->spinnervalue;
						continue;					
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 1 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 1 && curvenodes.size() > 0)
					{
						extraval = nodes[nodeinput]->spinnervalue;			
						continue;
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 2 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 2 && curvenodes.size() > 0)
					{
						extraval2 = nodes[nodeinput]->spinnervalue;			
						continue;
					}
					if (nodes[nodeinput]->name == "Int" && iinput1 == 3 && curvenodes.size() > 0
					 || nodes[nodeinput]->name == "Float" && iinput1 == 3 && curvenodes.size() > 0)
					{
						extraval3 = nodes[nodeinput]->spinnervalue;			
						continue;
					}
				}
			}
			if (curvenodes.size() > spinnervalue && spinnervalue >= 0 && extraval != -1 && extraval2 != -1 && extraval3 != -1)
			{
				FluidNode* node = new FluidNode(curvenodes[spinnervalue], PathMorph);
				node->extravalue = extraval;
				node->extravalue2 = extraval2;
				node->extravalue3 = extraval3;
				fluidnodes.push_back(node);
			}
		}
	}
}
