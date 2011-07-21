/*
 * DisambigMisc.cpp
 *
 *  Created on: Feb 5, 2011
 *      Author: ysun
 */

#include "DisambigPostProcess.h"

#include "DisambigCluster.h"
#include "DisambigFileOper.h"

extern "C" {
	# include "strcmp95.h"
}

#if 0
cCluster_Set & cCluster_Set::convert_from_ClusterInfo( const cCluster_Info * ps) {
	if ( ps == NULL )
		throw cException_Other("NULL pointer.");

	this->consolidated.clear();

	for ( map < string, cCluster_Info::cRecGroup >::const_iterator p = ps->get_cluster_map().begin(); p != ps->get_cluster_map().end(); ++p ) {
		for ( cCluster_Info::cRecGroup::const_iterator q = p->second.begin(); q != p->second.end(); ++q ) {
			//this->consolidated.insert(*q);
			this->consolidated.push_back(*q);
		}
	}
	return *this;
}
#endif

void find_associated_nodes(const cCluster & center, const map < const cRecord *, const cRecord *> & uid2uinv,
							const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree,
							set < const cRecord * > & associated_delegates) {
	associated_delegates.clear();
	for ( cGroup_Value::const_iterator p = center.get_fellows().begin(); p != center.get_fellows().end(); ++p ) {
		map < const cRecord *, cGroup_Value, cSort_by_attrib >::const_iterator ipat = patent_tree.find(*p);
		if ( ipat == patent_tree.end() ) {
			(*p)->print();
			throw cException_Attribute_Not_In_Tree("Cannot find the patent.");
		}

		for ( cGroup_Value::const_iterator cgi = ipat->second.begin(); cgi != ipat->second.end(); ++cgi ) {
			if ( *cgi == *p )
				continue;
			map < const cRecord *, const cRecord *>::const_iterator q = uid2uinv.find(*cgi);
			if ( q == uid2uinv.end() )
				throw cException_Attribute_Not_In_Tree("Cannot find the unique record pointer.");
			associated_delegates.insert(q->second);
		}

	}

	for ( cGroup_Value::const_iterator p = center.get_fellows().begin(); p != center.get_fellows().end(); ++p ) {
		associated_delegates.erase(*p);
	}
}

void post_polish( cCluster_Set & m, map < const cRecord *, const cRecord *> & uid2uinv,
					const map < const cRecord *, cGroup_Value, cSort_by_attrib > & patent_tree,
					const string & logfile) {
	std::cout << "Starting post processing ... ..." << std::endl;
	set < const cRecord *> linkages;
	const double normal_threshold = 0.95;
	const double asian_threshold = 0.99;
	double threshold = normal_threshold ;
	const unsigned int fi = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int li = cRecord::get_index_by_name(cLastname::static_get_class_name());
	const unsigned int country_index = cRecord::get_index_by_name(cCountry::static_get_class_name());
	const unsigned int uid_index = cRecord::get_index_by_name(cUnique_Record_ID::static_get_class_name());


	//const string logfile = "postprocesslog.txt";
	std::ofstream pplog(logfile.c_str());
	std::cout << "Saving post processing log to " << logfile << " ... ..." << std::endl;

	/*
	const unsigned int mi = cRecord::get_index_by_name(cMiddlename::static_get_class_name());


	vector < unsigned int > indice;
	indice.push_back(fi);
	indice.push_back(mi);
	indice.push_back(li);

	vector < unsigned int > compres;

	 */
	const unsigned int base = 1000;
	unsigned int cnt = 0;



	map < const cRecord *,  Cluster_Container::iterator > record2cluster;
	map < const cRecord *, Cluster_Container::iterator >::const_iterator z;
	for ( Cluster_Container ::iterator p = m.get_modifiable_set().begin(); p != m.get_modifiable_set().end(); ++p ) {
		record2cluster.insert( std::pair < const cRecord *, Cluster_Container::iterator > (p->get_cluster_head().m_delegate, p ) );
	}

	unsigned int round_cnt;

	do {
		round_cnt = cnt;
		for ( Cluster_Container ::const_iterator q = m.get_set().begin(); q != m.get_set().end(); ++q ) {
			const cAttribute * center_country_attrib = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(country_index);
			if ( ! center_country_attrib->is_informative() )
				continue;
			const string * centercountry = center_country_attrib->get_data().at(0);
			if ( *centercountry == "CN" || *centercountry == "JP" || *centercountry == "KR" || *centercountry == "TW"   )
				threshold = asian_threshold;


			linkages.clear();
			find_associated_nodes( *q, uid2uinv, patent_tree, linkages);
			list < const cRecord *> links ( linkages.begin(), linkages.end()) ;


			const cAttribute * center_first_attrib = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(fi);
			if ( center_first_attrib->get_data().empty() ) {
				q->get_cluster_head().m_delegate->print();
				throw cException_Other("Center First");
			}
			const string * centerfirst = center_first_attrib->get_data().at(0);

			const cAttribute * center_last_attrib = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(li);
			if ( center_last_attrib->get_data().empty() ) {
				q->get_cluster_head().m_delegate->print();
				throw cException_Other("Center Last");
			}
			const string * centerlast = center_last_attrib->get_data().at(0);

			const cAttribute * center_uid_attrib = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(uid_index);
			if ( center_uid_attrib->get_data().empty() ) {
				q->get_cluster_head().m_delegate->print();
				throw cException_Other("Center UID");
			}
			const string * centeruid = center_uid_attrib->get_data().at(0);

			//const string * centerlast = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(li)->get_data().at(0);
			//const string * centeruid = q->get_cluster_head().m_delegate->get_attrib_pointer_by_index(uid_index)->get_data().at(0);

			for ( list < const cRecord *>::iterator r = links.begin(); r != links.end(); ++r ) {
				//const string * pfirst = (*r)->get_attrib_pointer_by_index(fi)->get_data().at(0);
				//const string * plast = (*r)->get_attrib_pointer_by_index(li)->get_data().at(0);
				//const string * puid = (*r)->get_attrib_pointer_by_index(uid_index)->get_data().at(0);


				const cAttribute * pfirst_attrib = (*r)->get_attrib_pointer_by_index(fi);
				if ( pfirst_attrib->get_data().empty() ) {
					(*r)->print();
					throw cException_Other("P First");
				}
				const string * pfirst = pfirst_attrib->get_data().at(0);

				const cAttribute * plast_attrib = (*r)->get_attrib_pointer_by_index(li);
				if ( plast_attrib->get_data().empty() ) {
					(*r)->print();
					throw cException_Other("P Last");
				}
				const string * plast = plast_attrib->get_data().at(0);

				const cAttribute * puid_attrib = (*r)->get_attrib_pointer_by_index(uid_index);
				if ( puid_attrib->get_data().empty() ) {
					(*r)->print();
					throw cException_Other("P UID");
				}
				const string * puid = puid_attrib->get_data().at(0);

				list < const cRecord *>::iterator s = r;
				for ( ++s; s != links.end(); ) {
					//const string * qfirst = (*s)->get_attrib_pointer_by_index(fi)->get_data().at(0);
					//const string * qlast = (*s)->get_attrib_pointer_by_index(li)->get_data().at(0);
					//const string * quid = (*s)->get_attrib_pointer_by_index(uid_index)->get_data().at(0);

					const cAttribute * qfirst_attrib = (*s)->get_attrib_pointer_by_index(fi);
					if ( qfirst_attrib->get_data().empty() ) {
						(*s)->print();
						throw cException_Other("Q First");
					}
					const string * qfirst = qfirst_attrib->get_data().at(0);


					const cAttribute * qlast_attrib = (*s)->get_attrib_pointer_by_index(li);
					if ( qlast_attrib->get_data().empty() ) {
						(*s)->print();
						throw cException_Other("Q Last");
					}
					const string * qlast = qlast_attrib->get_data().at(0);

					const cAttribute * quid_attrib = (*s)->get_attrib_pointer_by_index(uid_index);
					if ( quid_attrib->get_data().empty() ) {
						(*s)->print();
						throw cException_Other("Q UID");
					}
					const string * quid = quid_attrib->get_data().at(0);



					double first_score = strcmp95_modified(pfirst->c_str(), qfirst->c_str() );
					double last_score = strcmp95_modified(plast->c_str(), qlast->c_str());

					if ( first_score > threshold  && last_score > threshold  ) {
						pplog << *pfirst << "." << *plast << " = " << *qfirst << "." << *qlast << " <----- " << *centerfirst << "."<< *centerlast << "        ||       ";
						pplog << *puid << " = " << * quid << " <----- " << *centeruid << std::endl;

						z = record2cluster.find(*r);
						if ( z == record2cluster.end() ) {
							//++s;
							//continue;
							throw cException_Attribute_Not_In_Tree("Record pointer not in tree.");
						}
						Cluster_Container::iterator pmerger = z->second;

						z = record2cluster.find(*s);
						if ( z == record2cluster.end() ) {
							//++s;
							//continue;
							throw cException_Attribute_Not_In_Tree("Record pointer not in tree.");
						}
						Cluster_Container::iterator pmergee = z->second;

						// need to do 4 things: merge, delete/update from cluster_set, update record2cluster, and update uid2uiv map

						//1, merge
						pmerger->merge(*pmergee, pmerger->get_cluster_head());

						//2, delete from record2cluster;
						const cRecord * newhead = pmerger->get_cluster_head().m_delegate;
						record2cluster.erase(*s);
						if ( *r != newhead ) {
							record2cluster.erase(*r);
							record2cluster.insert( std::pair < const cRecord *, Cluster_Container::iterator >(newhead, pmerger ));
							*r = newhead;
						}


						//3, update the uid2uinv map;
						for ( cGroup_Value::const_iterator p = pmerger->get_fellows().begin(); p != pmerger->get_fellows().end(); ++p ) {
							map < const cRecord *, const cRecord *>::iterator t = uid2uinv.find(*p);
							if ( t == uid2uinv.end() )
								throw cException_Attribute_Not_In_Tree("Record pointer not in uid2uinv tree.");
							t->second = pmerger->get_cluster_head().m_delegate;
						}

						//4, delete from cluster_set
						m.get_modifiable_set().erase(pmergee);


						links.erase(s++);
						++cnt;
						if ( cnt % base == 0 )
							std::cout << cnt << " records have been polished." << std::endl;
					}
					else
						++s;
				}
			}
			if ( *centercountry == "CN" || *centercountry == "JP" || *centercountry == "KR" || *centercountry == "TW"   )
				threshold = normal_threshold;
		}
	} while ( round_cnt != cnt );


	std::cout << cnt << " records have been polished." << std::endl;

}

void cCluster_Set::output_results( const char * dest_file) const {
	std::cout << "Writing to " << dest_file << " ... ...";
	std::ostream::sync_with_stdio(false);
	const string & uid_name = cUnique_Record_ID::static_get_class_name();
	const unsigned int uid_index = cRecord::get_index_by_name(uid_name);
	static const cException_Vector_Data except(uid_name.c_str());

	std::ofstream os(dest_file);

	for ( Cluster_Container ::const_iterator p = this->consolidated.begin(); p != this->consolidated.end(); ++p ) {

		const cAttribute * key_pattrib = p->get_cluster_head().m_delegate->get_attrib_pointer_by_index(uid_index);

		os << * key_pattrib->get_data().at(0) << cCluster_Info::primary_delim;
		double cohesion_value = p->get_cluster_head().m_cohesion;
		os << cohesion_value << cCluster_Info::primary_delim;

		for ( cGroup_Value::const_iterator q = p->get_fellows().begin(); q != p->get_fellows().end(); ++q ) {
			const cAttribute * value_pattrib = (*q)->get_attrib_pointer_by_index(uid_index);

			os << * value_pattrib->get_data().at(0) << cCluster_Info::secondary_delim;
		}
		os << '\n';
	}
	std::ostream::sync_with_stdio(true);
	std::cout << "Done." << std::endl;
}

void cCluster_Set::read_from_file( const char * filename, const map <string, const cRecord*> & uid_tree) {
	unsigned int count = 0;
	const unsigned int base = 100000;
	const unsigned int primary_delim_size = strlen(cCluster_Info::primary_delim);
	const unsigned int secondary_delim_size = strlen(cCluster_Info::secondary_delim);
	std::ifstream infile ( filename);
	if (infile.good()) {
		string filedata;
		while ( getline(infile, filedata)) {
			register size_t pos = 0, prev_pos = 0;
			pos = filedata.find(cCluster_Info::primary_delim, prev_pos);
			string keystring = filedata.substr( prev_pos, pos - prev_pos);
			const cRecord * key = retrieve_record_pointer_by_unique_id( keystring, uid_tree );
			prev_pos = pos + primary_delim_size;

			pos = filedata.find(cCluster_Info::primary_delim, prev_pos);
			double val = 0;
			if ( true ) {
				string cohesionstring = filedata.substr( prev_pos, pos - prev_pos);
				val = atof(cohesionstring.c_str());
			}
			prev_pos = pos + primary_delim_size;


			cGroup_Value tempv;
			while ( ( pos = filedata.find(cCluster_Info::secondary_delim, prev_pos) )!= string::npos){
				string valuestring = filedata.substr( prev_pos, pos - prev_pos);
				const cRecord * value = retrieve_record_pointer_by_unique_id( valuestring, uid_tree);
				tempv.push_back(value);
				prev_pos = pos + secondary_delim_size;
			}
			cCluster_Head th(key, val);
			cCluster tempc(th, tempv);
			tempc.self_repair();
			this->consolidated.push_back(tempc);

			++count;
			if ( count % base == 0 )
				std::cout << count << " records have been loaded from the cluster file. " << std::endl;
		}
		std::cout << "Totally, " << count << " records have been loaded from " << filename << std::endl;
	}
	else {
		throw cException_File_Not_Found(filename);
	}
}


