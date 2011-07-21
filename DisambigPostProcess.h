/*
 * DisambigMisc.h
 *
 *  Created on: Feb 5, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGMISC_H_
#define DISAMBIGMISC_H_
#include "DisambigNewCluster.h"

class cCluster_Info;

struct cSort_For_Cluster {
	bool operator () ( const cCluster & c1, const cCluster & c2 ) const {
		return c1.get_cluster_head().m_delegate < c2.get_cluster_head().m_delegate;
	}
};


//typedef set < cCluster, cSort_For_Cluster > Cluster_Container;
typedef list < cCluster > Cluster_Container;


class cCluster_Set {
private:
	Cluster_Container consolidated;
	cCluster_Set ( const cCluster_Set &);
public:
	//cCluster_Set & convert_from_ClusterInfo( const cCluster_Info * );
	const Cluster_Container & get_set() const {return consolidated;}
	Cluster_Container  & get_modifiable_set() { return consolidated; }
	cCluster_Set () {}
	~cCluster_Set() {}
	void output_results( const char * ) const;
	void read_from_file ( const char * filename, const map <string, const cRecord*> & uid_tree);
};

void post_polish( cCluster_Set & m, map < const cRecord *, const cRecord *> & uid2uinv,
					const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree,
					const string & logfile);







#endif /* DISAMBIGMISC_H_ */
