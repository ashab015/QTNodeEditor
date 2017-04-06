#include "FluidNodeEditor.h"


void FluidNodeEditorWidget::DrawBackground(QPaintEvent *event)
{

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// draw background
	float backgroundgray = 80;
	QColor darkbluecolor(0, 38, 77);
	painter.setBrush(QColor(backgroundgray, backgroundgray, backgroundgray));
	QRectF rec(0, 0, this->width(), this->height());
	painter.drawRect(rec);

	// cross
	painter.setBrush(QBrush(QColor(0, 0, 0, 20), Qt::BrushStyle::CrossPattern));
	painter.drawRect(rec);

}

void FluidNodeEditorWidget::DrawSideMenu(QPaintEvent *event)
{




}

void FluidNodeEditorWidget::DrawNodes(QPaintEvent *event)
{

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Draw all nodes
	for (int x = 0; nodes.size() > x; x++)
	{
		float smooth = 6;
		float nodeheight = 180;
		int shadowoffset = 6;
		painter.setPen(Qt::NoPen);
		nodeheight = (inputspacing * std::max(nodes[x]->inputs.size() + 1, nodes[x]->outputs.size() + 1)) + inputspacing / 4;
		QRectF recdrop(nodes[x]->position.x() + shadowoffset, nodes[x]->position.y() + shadowoffset, nodewidth, nodeheight);
		painter.setBrush(QBrush(QColor(0, 0, 0, 80)));
		painter.drawRoundedRect(recdrop, smooth, smooth);

		if (selectednode == x)
		{
			painter.setBrush(Qt::white);
			float inputspacing = 26;
			float nodewidth = 120;
			float nodeheight = 180;
			nodeheight = (inputspacing * std::max(nodes[x]->inputs.size() + 1, nodes[x]->outputs.size() + 1)) + inputspacing / 4;
			float selectsize = 4;
			nodeheight += selectsize;
			nodewidth += selectsize;
			QRectF rec(nodes[x]->position.x() - selectsize / 2, nodes[x]->position.y() - selectsize / 2, nodewidth, nodeheight);
			painter.drawRoundedRect(rec, smooth, smooth);
		}

		// Draw background
		painter.setPen(QColor(0, 0, 0, 255));
		painter.setBrush(nodes[x]->nodecolor);
		float inputspacing = 26;
		float nodewidth = 120;
		QColor gradientscalar = QColor(20, 20, 20);
		QRectF rec(nodes[x]->position.x(), nodes[x]->position.y(), nodewidth, nodeheight);
		QLinearGradient gradient((rec.topLeft() + rec.topRight()) / 2, (rec.bottomLeft() + rec.bottomRight()) / 2); // diagonal gradient from top-left to bottom-right
		gradient.setColorAt(0, nodes[x]->nodecolor);
		gradient.setColorAt(1, nodes[x]->nodecolor.darker(200));
		painter.drawRoundedRect(rec, smooth, smooth);

		// name title
		painter.setBrush(Qt::darkGray);
		QRectF rec2(nodes[x]->position.x(), nodes[x]->position.y(), nodewidth, fontsize * 3);
		painter.drawRoundedRect(rec2, smooth, smooth);
		//painter.drawRect(rec2);
		painter.setBrush(QColor(20, 20, 20));
		painter.setFont(QFont("Helvetica", nodes[x]->fontsize * 1.6));
		painter.setPen(QColor(0, 0, 0, 255));
		QRectF rec3((textpadding / 2) + nodes[x]->position.x(), nodes[x]->position.y(), nodewidth, fontsize * 3);
		painter.drawText(rec3, QString(nodes[x]->name.c_str()));

		// inputs
		painter.setFont(QFont("Helvetica", fontsize * 0.9));
		for (int i = 1; nodes[x]->inputs.size() + 1 > i; i++)
		{
			painter.drawText(QPoint(textpadding + nodes[x]->position.x(), ((titleheight * 1.2) + nodes[x]->position.y()) + (inputspacing * i)), QString(nodes[x]->inputs[i - 1].c_str()));
			QPoint inputpos(textpadding + nodes[x]->position.x(), ((titleheight * 1.2) + nodes[x]->position.y()) + (inputspacing * i));
			inputpos.setX(inputpos.x() - 8);
			inputpos.setY(inputpos.y() - 4);
			painter.setBrush(Qt::gray);
			painter.drawEllipse(inputpos, connectionradius + 3, connectionradius + 3);
			painter.setBrush(nodes[x]->inputcolors[i - 1]);
			painter.drawEllipse(inputpos, connectionradius, connectionradius);
		}

		// outputs
		for (int i = 1; nodes[x]->outputs.size() + 1 > i; i++)
		{
			painter.drawText(QPoint((nodes[x]->position.x() + (nodewidth / 1.5)), ((titleheight * 1.2) + nodes[x]->position.y()) + (inputspacing * i)), QString(nodes[x]->outputs[i - 1].c_str()));
			QPoint inputpos((nodes[x]->position.x() + (nodewidth / 1.5)), ((titleheight * 1.2) + nodes[x]->position.y()) + (inputspacing * i));
			inputpos.setX(inputpos.x() + 40);
			inputpos.setY(inputpos.y() - 4);
			painter.setBrush(Qt::gray);
			painter.drawEllipse(inputpos, connectionradius + 3, connectionradius + 3);
			painter.setBrush(nodes[x]->outputcolors[i - 1]);
			painter.drawEllipse(inputpos, connectionradius, connectionradius);
		}

		if (nodes[x]->spinner == true)
		{
			painter.setBrush(Qt::white);
			QRectF rec(nodes[x]->position.x() + 4, nodes[x]->position.y() + 34, 52, 22);
			painter.drawRect(rec);

			painter.setPen(QColor(0, 0, 0, 255));
			painter.setFont(QFont("Helvetica", 9 * 1.6));
			std::string value = std::to_string(nodes[x]->spinnervalue).c_str();
			value[3] = ' ';
			value[4] = ' ';
			value[5] = ' ';
			value[6] = ' ';
			value[7] = ' ';

			painter.drawText(nodes[x]->position.x() + 6, nodes[x]->position.y() + 52, value.c_str());

			rec2 = QRectF(nodes[x]->position.x() + 56, nodes[x]->position.y() + 34, 17, 11);
			nodes[x]->rec1 = rec2;
			painter.drawRect(rec2);
			rec3 = QRectF(nodes[x]->position.x() + 56, nodes[x]->position.y() + 45, 17, 11);
			nodes[x]->rec2 = rec3;
			painter.drawRect(rec3);

			painter.setFont(QFont("Helvetica", 5 * 1.6));
			painter.drawText(nodes[x]->position.x() + 61, nodes[x]->position.y() + 43, ">");
			painter.drawText(nodes[x]->position.x() + 61, nodes[x]->position.y() + 54, "<");

		}

	}



}