#pragma once
#include "NodeEditor.h"
#include <QtWidgets\QGraphicsBlurEffect>
#include <string>

/// <summary> Widget to display the node editor for the fluid. </summary>
class FluidNodeEditorWidget : public NodeEditorWidget
{
public:
	// Draw functions
	FluidNodeEditorWidget(std::string name, std::string path)
	{
		exportname = name;
		loadpath = path;
		savepath = path;
		LoadEditor(name + ".cfx");
	}
	void DrawBackground(QPaintEvent *event);
	void DrawSideMenu(QPaintEvent *event);
	void DrawNodes(QPaintEvent *event);
};
