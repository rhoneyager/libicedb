#pragma once


struct icedb_GlobalTestingFixture {
	icedb_GlobalTestingFixture();
	void setup();
	void teardown();
	virtual ~icedb_GlobalTestingFixture();
};
