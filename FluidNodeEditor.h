#pragma once
#include "NodeEditor.h"
#include <QtWidgets\QGraphicsBlurEffect>
#include <string>

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