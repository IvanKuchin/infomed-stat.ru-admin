#include "admin.h"

int main()
{
	CStatistics		appStat;  // --- CStatistics must be first statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	c_config		config(CONFIG_DIR);
	CMysql			db;
	string			act, id, content;
	struct timeval	tv;

	{
		CLog	log(ADMIN_LOG_FILE_NAME);
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + "]: " + __FILE__);
	}

	signal(SIGSEGV, crash_handler); 

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);  /* Flawfinder: ignore */
	appStat.SetLogFile(ADMIN_LOG_FILE_NAME);

	try
	{

		indexPage.ParseURL();

		if(!indexPage.SetAdminTemplate("/admin_index.htmlt"))
		{
			CLog	log(ADMIN_LOG_FILE_NAME);

			log.Write(ERROR, string(__func__) + ": ERROR: template file was missing");
			throw CException("Template file was missing");
		}

 
		if(db.Connect(&config) < 0)
		{
			CLog	log(ADMIN_LOG_FILE_NAME);

			log.Write(ERROR, string(__func__) + ": ERROR: Can not connect to mysql database");
			return(1);
		}

		act = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("act"));

		indexPage.RegisterVariable("rand", GetRandom(10).c_str());

		{
			CLog	log(ADMIN_LOG_FILE_NAME);
			log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + "]: act ==", act);
		}

			
		indexPage.RegisterVariableForce("rand", GetRandom(10).c_str());
		indexPage.OutTemplate();


	}
	catch(CException c)
	{
		indexPage.SetAdminTemplate("/error.htmlt");
		indexPage.RegisterVariable("content", c.GetReason().c_str());
		indexPage.OutTemplate();
	return(-1);
	}

	return(0);
}

