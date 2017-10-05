#include <StdAfx.h>
#include <OpenDivaCommon.h>
#include <SQLite\SQLiteBus.h>
#include <AzCore\std\parallel\mutex.h>

#include "JudgeList.h"

#include "../Judge/DivaJudgeFactory.h"

#include <ctime>

namespace OpenDiva {
	AZStd::mutex JudgeList::m_mutex;

	void JudgeList::Refresh() {
		time_t now = time(0);
		AZ::Crc32 crchash = AZ::Crc32(ctime(&now));
		AZStd::string crchash_str = Util::Crc32ToString(crchash);

		SQLite3::SQLiteDB * sysDb;
		SQLITE_BUS(sysDb, AZ::EntityId(0), GetConnection); //get system db
		AZ_Assert(sysDb, "sysDb is null.");

		sysDb->Exec("BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

		//list judges
		std::vector<std::string> judges = DivaJudgeFactory::getFactory().getJudges();

		for (std::string judge : judges) {
			AZ::Crc32 judgecrc = AZ::Crc32(judge.c_str());
			judgecrc.Add(OPENDIVA_SALT);

			//add to database
			/*
			juuid
			name
			description
			version
			type
			dbcrc
			*/
			AZStd::string judge_sql_str =
				"INSERT OR REPLACE INTO Judges ("
					"juuid,"
					"name,"
					"type,"
					"dbcrc"
				") VALUES ("
					"\"" + Util::Crc32ToString(judgecrc) + "\","
					"\"" + AZStd::string(judge.c_str()) + "\","
					"\"C++\","
					"\"" + crchash_str + "\""
				");";

			sysDb->Exec(judge_sql_str.c_str(), nullptr, nullptr, nullptr);
		}

		//scan for lua judges

		sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);

		//database cleanup
		AZStd::string cleanup = "DELETE FROM Judges WHERE dbcrc != \"" + crchash_str + "\";";

		//sysDb->Exec("BEGIN TRANSACTION", nullptr, nullptr, nullptr);
		sysDb->Exec(cleanup.c_str(), nullptr, nullptr, nullptr);
		//sysDb->Exec("END TRANSACTION", nullptr, nullptr, nullptr);
	}
}