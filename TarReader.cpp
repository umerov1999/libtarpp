#include "TarReader.hpp"
#include "TarContents.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <memory>
#ifdef __linux__
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#endif
using namespace std;

void libtarpp::TarReader::load(const string& filename)
{
	contents.clear();
	ifstream ifs(filename);
	long filesize=ifs.seekg(0,ios::end).tellg();
	ifs.seekg(0,ios::beg);
	string header;
	for(int i=1;i<=512;i++)
	{
		header+=ifs.get();
		if(i==512)
		{
			if(header.substr(0,1)[0]=='\0')
			{
				break;
			}

			TarContents t;
			t.setName(header.substr(0,100));
			t.setMode(header.substr(100,8));
			t.setUid(header.substr(108,8));
			t.setGid(header.substr(116,8));
			t.setRawSize(header.substr(124,12));
			t.setMTime(header.substr(136,12));
			t.setChkSum(header.substr(148,8));
			t.setTypeFlag(header.substr(156,1));
			t.setLinkName(header.substr(157,100));
			//header.substr(257,6);
			//header.substr(263,2);
			t.setUName(header.substr(265,32));
			t.setGName(header.substr(297,32));
			t.setDevMajor(header.substr(329,8));
			t.setDevMinor(header.substr(337,8));
			t.setDevMinor(header.substr(345,155));

			shared_ptr<fstream> strm(new fstream(filename,ios_base::in));
			//strm->open(filename,std::ifstream::in);
			strm->rdbuf()->pubseekoff(ifs.tellg(),ios::beg);
			//cout<<strm->rdbuf()->sgetc()<<endl;
			//cout<<hex<<strm<<endl;
			t.setStream(strm);
		
			contents.insert(contents.begin(),t);
			cout<<t.getName()<<endl;
			//cout<<"current:"<<ifs.tellg()<<endl;

			header = "";
			long sz = stoi(t.getRawSize().substr(0,11),nullptr,8);
			if((long)(ifs.tellg())+sz+(512-sz%512)+512+512*2>filesize)
			{
				break;
			}
			ifs.seekg((512-sz%512)+sz,ios::cur);
			i=0;
			header == "";
		}
	}
}

string libtarpp::TarReader::readText(const string& path)
{
	TarContents t = getContents(path);
	shared_ptr<istream> stream = t.getStream();
	stream->rdbuf()->sbumpc();
	return "";
}

libtarpp::TarContents libtarpp::TarReader::getContents(const string& path)
{
	for(auto i : contents)
	{
		string tmp;
		for(auto c : i.getName())
		{
			if(c!='\0')
			{
				tmp+=c;
			}
		}
		if(tmp==path)
		{
			return i;
		}
	}
	cout<<"nasi "<<endl;
	throw "";
}

