#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include "NoteFileEntries.h"

#define TO_CRY_STRING(x) AZStd::string(std::to_string(x).c_str())

#define getAttrData(node, tag, variable) \
	if (##node##->haveAttr(##tag##)) { \
		node##->getAttr(##tag, variable##); \
	} else { \
		AZStd::string err = "cannot find path attributes: "; \
		err += tag##; \
		err += " - Line: " + node##->getLine(); \
		ret.errors.push_back(err); \
		ret.malformed = true; \
	}

namespace LYGame {

	//<bpm id="" val="60" time="" />
	NoteEntrySerializeError NoteEntryBPM::Serialize(XmlNodeRef node) {
        this->sType = eST_Norm;
                
		NoteEntrySerializeError ret;
		//if (node->haveAttr("val") && /*node->haveAttr("beats") &&*/ node->haveAttr("time")) {
		//	node->getAttr("val", this->bpm);
		//	//node->getAttr("beats", this->beats);
		//	node->getAttr("time", this->time);
		//} else {
		//	ret.malformed = true;
		//	string err = "bpm tag is missing val or time attributes.";
		//	err += " - Line: " + node->getLine();
		//	ret.errors.push_back(err);
		//}

		getAttrData(node, "val",this->bpm)
		getAttrData(node, "time", this->time)

		return ret;
	}

	AZStd::string NoteEntryBPM::toString() {
		AZStd::string ret = "";
		ret += "ID: " + TO_CRY_STRING(this->id);
		ret += "\n- time: " + TO_CRY_STRING(this->time);
		ret += "\n- bpm: " + TO_CRY_STRING(this->bpm);
		ret += "\n- delay ms: " + TO_CRY_STRING(this->getMSDelay());
		ret += "\n- delay sec: " + TO_CRY_STRING(this->getDelay());
		return ret;
	}

	/*
	//section is optional
	<note id="" time="" type="" section="tech/chance">
		<bcurve pos="x,y" angle="" ctrDist1="0.5" ctrDist2="" />
	</note>
	<note id="" time="" type="" section="tech/chance">
		<line pos="x,y" angle="" />
	</note>
	*/
	NoteEntrySerializeError NoteEntrySingle::Serialize(XmlNodeRef node) {
        this->sType = eST_Norm;
                
		NoteEntrySerializeError ret;
                
        if (node->haveAttr("section")){
			AZStd::string section = node->getAttr("section");
                    
            if (section.compare("tech") == 0) this->sType = eST_Tech;
            else if (section.compare("chance") == 0) this->sType = eST_Chance;
            else{
				AZStd::string err = "invalid section type";
                err += " - Line: " + node->getLine();
                ret.errors.push_back(err);
                ret.malformed = true;
            }
        }

		if (node->haveAttr("time") && node->haveAttr("type")) {
			node->getAttr("time", this->time);
			AZStd::string type = node->getAttr("type");

			if (type.compare("cross") == 0 || type.compare("x") == 0 || type.compare("X") == 0) this->type = eNT_Cross;
			else if (type.compare("circle") == 0 || type.compare("o") == 0 || type.compare("O") == 0) this->type = eNT_Circle;
			else if (type.compare("square") == 0 || type.compare("[]") == 0) this->type = eNT_Square;
			else if (type.compare("triangle") == 0 || type.compare("/\\") == 0) this->type = eNT_Triangle;
			/*
			else if (type.compare("left") == 0 || type.compare("<") == 0) this->type = eNT_Left;
			else if (type.compare("right") == 0 || type.compare(">") == 0) this->type = eNT_Right;
			else if (type.compare("up") == 0 || type.compare("^") == 0) this->type = eNT_Up;
			else if (type.compare("down") == 0 || type.compare("v") == 0 ||  || type.compare("V") == 0) this->type = eNT_Down;
			*/
			else {
				AZStd::string err = "invalid note type";
				err += " - Line: " + node->getLine();
				ret.errors.push_back(err);
				ret.malformed = true;
			}

			if (!ret.malformed) {
				if (node->getChildCount() == 1) {
					XmlNodeRef path = node->getChild(0);
					AZStd::string pathTag = path->getTag();

					if (pathTag.compare("bcurve") == 0) {
						this->pType = ePT_BCurve;
						Vec2 position;
						getAttrData(path, "pos", position)
						this->pos = AZ::Vector2(position.x, position.y);
						getAttrData(path,"angle", this->angle)
						getAttrData(path, "ctrDist1", this->ctrlDist1)
						getAttrData(path, "ctrDist2", this->ctrlDist2)
					} else if (pathTag.compare("line") == 0) {
						this->pType = ePT_Line;
						Vec2 position;
						getAttrData(path, "pos", position)
						this->pos = AZ::Vector2(position.x, position.y);
						getAttrData(path, "angle", this->angle)
					} else {
							AZStd::string err = "invalid path tag";
						err += " - Line: " + path->getLine();
						ret.errors.push_back(err);
						ret.malformed = true;
					}
				} else {
					AZStd::string err = "tag doesnt have a path child or has to many childs";
					err += " - Line: " + node->getLine();
					ret.errors.push_back(err);
					ret.malformed = true;
				}
			}
		} else {
			AZStd::string err = "cannot find time or type attributes";
			err += " - Line: " + node->getLine();
			ret.errors.push_back(err);
			ret.malformed = true;
		}

		return ret;
	}

	AZStd::string NoteEntrySingle::toString() {
		AZStd::string ret = "";
		ret += "ID: " + TO_CRY_STRING(this->id);
		ret += "\n- time: " + TO_CRY_STRING(this->time);
		ret += "\n- type: " + TO_CRY_STRING(this->type);
		ret += "\n- pType: " + TO_CRY_STRING(this->pType);
        ret += "\n- sType: " + TO_CRY_STRING(this->sType);

		switch (pType) {
		case ePT_BCurve:
			ret += "\n-- Pos:";
			ret += "\n--- x: " + TO_CRY_STRING(this->pos.GetX());
			ret += "\n--- y: " + TO_CRY_STRING(this->pos.GetY());
			ret += "\n-- Angle: " + TO_CRY_STRING(this->angle);
			ret += "\n-- ctlDists: ";
			ret += "\n--- 1: " + TO_CRY_STRING(this->ctrlDist1);
			ret += "\n--- 2: " + TO_CRY_STRING(this->ctrlDist2);
			break;
		case ePT_Line:
			ret += "\n-- Pos:";
			ret += "\n--- x: " + TO_CRY_STRING(this->pos.GetX());
			ret += "\n--- y: " + TO_CRY_STRING(this->pos.GetY());
			ret += "\n-- Angle: " + TO_CRY_STRING(this->angle);
			break;
		}

		return ret;
	}

	/*
	//section is optional
	<hold id="" time1="" time2="" type="" section="tech/chance">
		<bcurve pos="x,y" angle="" ctrDist1="0.5" ctrDist2="" />
	</hold>
  
	<hold id="" time1="" time2="" type="" section="tech/chance">
		<line pos="x,y" angle="" />
	</hold>
	*/
	NoteEntrySerializeError NoteEntryHold::Serialize(XmlNodeRef node) {
        this->sType = eST_Norm;
                
		NoteEntrySerializeError ret;
                
        if (node->haveAttr("section")){
            string section = node->getAttr("section");
                    
            if (section.compare("tech") == 0) this->sType = eST_Tech;
            else if (section.compare("chance") == 0) this->sType = eST_Chance;
            else{
				AZStd::string err = "invalid section type";
                err += " - Line: " + node->getLine();
                ret.errors.push_back(err);
                ret.malformed = true;
            }
        }

		if (node->haveAttr("time1") && node->haveAttr("time2") && node->haveAttr("type")) {
			node->getAttr("time1", this->hold1);
			node->getAttr("time2", this->hold2);
			AZStd::string type = node->getAttr("type");

			if (type.compare("cross") == 0 || type.compare("x") == 0 || type.compare("X") == 0) this->type = eNT_Cross;
			else if (type.compare("circle") == 0 || type.compare("o") == 0 || type.compare("O") == 0) this->type = eNT_Circle;
			else if (type.compare("square") == 0 || type.compare("[]") == 0) this->type = eNT_Square;
			else if (type.compare("triangle") == 0 || type.compare("/\\") == 0) this->type = eNT_Triangle;
			else {
				AZStd::string err = "invalid note type";
				err += " - Line: " + node->getLine();
				ret.errors.push_back(err);
				ret.malformed = true;
			}

			if (!ret.malformed) {
				if (node->getChildCount() == 1) {
					XmlNodeRef path = node->getChild(0);
					AZStd::string pathTag = path->getTag();

					if (pathTag.compare("bcurve") == 0) {
						this->pType = ePT_BCurve;
						Vec2 position;
						getAttrData(path, "pos", position)
						this->pos = AZ::Vector2(position.x, position.y);
						getAttrData(path, "angle", this->angle)
						getAttrData(path, "ctrDist1", this->ctrlDist1)
						getAttrData(path, "ctrDist2", this->ctrlDist2)
					} else if (pathTag.compare("line") == 0) {
						this->pType = ePT_Line;
						Vec2 position;
						getAttrData(path, "pos", position)
						this->pos = AZ::Vector2(position.x, position.y);
						getAttrData(path, "angle", this->angle)
					} else {
							AZStd::string err = "invalid path tag";
						err += " - Line: " + path->getLine();
						ret.errors.push_back(err);
						ret.malformed = true;
					}
				} else {
					AZStd::string err = "tag doesnt have a path child or has to many childs";
					err += " - Line: " + node->getLine();
					ret.errors.push_back(err);
					ret.malformed = true;
				}
			}
		} else {
			AZStd::string err = "cannot find times or type attributes";
			err += " - Line: " + node->getLine();
			ret.errors.push_back(err);
			ret.malformed = true;
		}

		return ret;
	}

	AZStd::string NoteEntryHold::toString() {
		AZStd::string ret = "";
		ret += "ID: " + TO_CRY_STRING(this->id);
		ret += "\n- time1: " + TO_CRY_STRING(this->hold1);
		ret += "\n- time2: " + TO_CRY_STRING(this->hold2);
		ret += "\n- type: " + TO_CRY_STRING(this->type);
		ret += "\n- pType: " + TO_CRY_STRING(this->pType);
        ret += "\n- sType: " + TO_CRY_STRING(this->sType);

		switch (pType) {
		case ePT_BCurve:
			ret += "\n-- Pos:";
			ret += "\n--- x: " + TO_CRY_STRING(this->pos.GetX());
			ret += "\n--- y: " + TO_CRY_STRING(this->pos.GetY());
			ret += "\n-- Angle: " + TO_CRY_STRING(this->angle);
			ret += "\n-- ctlDists: ";
			ret += "\n--- 1: " + TO_CRY_STRING(this->ctrlDist1);
			ret += "\n--- 2: " + TO_CRY_STRING(this->ctrlDist2);
			break;
		case ePT_Line:
			ret += "\n-- Pos:";
			ret += "\n--- x: " + TO_CRY_STRING(this->pos.GetX());
			ret += "\n--- y: " + TO_CRY_STRING(this->pos.GetY());
			ret += "\n-- Angle: " + TO_CRY_STRING(this->angle);
			break;
		}

		return ret;
	}
}

#undef TO_CRY_STRING
#undef getAttrData