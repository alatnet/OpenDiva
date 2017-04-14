#include <StdAfx.h>
#include "SongInfo.h"

namespace LYGame {
	SongInfo::SongInfo(string path) {
		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(path);
		//read file

		if (file != 0) {
			this->m_info = SongInfo::GetInfo(file);

			if (this->m_info.valid) {
			}
		}
	}

	SongInfo::~SongInfo() {
	}

	SongFileInfo SongInfo::GetInfo(const char * filename) {
		SongFileInfo ret;

		XmlNodeRef file = gEnv->pSystem->LoadXmlFromFile(filename);

		if (file != 0) {
			XmlNodeRef name = file->findChild("name");
			ret.name = name->getContent();
			ret.nameR = name->getAttr("romaji");
			ret.nameE = name->getAttr("english");

			XmlNodeRef authors = file->findChild("authors");

			for (unsigned int i = 0; i < authors->getChildCount(); i++) {
				XmlNodeRef author = authors->getChild(i);

				SongFileInfo::Author authorInfo;

				authorInfo.name = author->getContent();
				authorInfo.nameR = author->getAttr("romaji");
				authorInfo.nameE = author->getAttr("english");

				ret.authors.push_back(std::make_pair<>(string(author->getTag()), authorInfo));
			}

			XmlNodeRef desc = file->findChild("desc");
			if (desc != 0) ret.desc = desc->getContent();

			XmlNodeRef albumArt = file->findChild("albumArt");
			if (albumArt != 0) ret.albumArtPath = albumArt->getContent();

			XmlNodeRef bpm = file->findChild("bpm");
			if (bpm->getChildCount() == 2) {
				XmlNodeRef bpmstart = bpm->findChild("start");
				XmlNodeRef bpmend = bpm->findChild("end");

				ret.bpm = std::make_pair<>(atoi(bpmstart->getContent()), atoi(bpmend->getContent()));
			} else {
				int bpmret = atoi(bpm->getContent());
				ret.bpm = std::make_pair<>(bpmret, bpmret);
			}

			ret.valid = true;
		}

		return ret;
	}

	SongFileInfo SongInfo::GetInfo(XmlNodeRef node) {
		SongFileInfo ret;

		if (node != 0) {
			XmlNodeRef name = node->findChild("name");
			ret.name = name->getContent();
			ret.nameR = name->getAttr("romaji");
			ret.nameE = name->getAttr("english");

			XmlNodeRef authors = node->findChild("authors");

			for (unsigned int i = 0; i < authors->getChildCount(); i++) {
				XmlNodeRef author = authors->getChild(i);

				SongFileInfo::Author authorInfo;

				authorInfo.name = author->getContent();
				authorInfo.nameR = author->getAttr("romaji");
				authorInfo.nameE = author->getAttr("english");

				ret.authors.push_back(std::make_pair<>(author->getTag(), authorInfo));
			}

			XmlNodeRef desc = node->findChild("desc");
			if (desc != 0) ret.desc = desc->getContent();

			XmlNodeRef albumArt = node->findChild("albumArt");
			if (albumArt != 0) ret.albumArtPath = albumArt->getContent();

			XmlNodeRef bpm = node->findChild("bpm");
			if (bpm->getChildCount() == 2) {
				XmlNodeRef bpmstart = bpm->findChild("start");
				XmlNodeRef bpmend = bpm->findChild("end");

				ret.bpm = std::make_pair<>(atoi(bpmstart->getContent()), atoi(bpmend->getContent()));
			} else {
				int bpmret = atoi(bpm->getContent());
				ret.bpm = std::make_pair<>(bpmret, bpmret);
			}

			ret.valid = true;
		}

		return ret;
	}
}