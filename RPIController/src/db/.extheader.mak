db.d: ../../include/RPIMgr.h 
../../include/RPIMgr.h: RPIMgr.h
	@rm -f ../../include/RPIMgr.h
	@ln -fs ../src/db/RPIMgr.h ../../include/RPIMgr.h
