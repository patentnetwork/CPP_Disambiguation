/*
 * DisambigCluster.cpp
 *
 *  Created on: Jan 12, 2011
 *      Author: ysun
 */

#include "DisambigCluster.h"
#include "DisambigEngine.h"
#include "DisambigFileOper.h"
#include "DisambigRatios.h"
#include "DisambigNewCluster.h"
extern "C" {
#include "strcmp95.h"
}
#include <cmath>


const char * cCluster_Info::primary_delim = "###";
const char * cCluster_Info::secondary_delim = ",";

bool cCluster_Info::is_consistent() const {
	unsigned int temp_total = 0;
	for ( map < string, cRecGroup >::const_iterator cp = cluster_by_block.begin(); cp != cluster_by_block.end(); ++cp ) {
		for ( cRecGroup::const_iterator cq = cp->second.begin(); cq != cp ->second.end(); ++ cq ) {
			//temp_total += cq->second.size();
			temp_total += cq->get_fellows().size();
		}
		//if ( cohesion_map_by_block.find(&cp->first)->second.size() != cp->second.size())
		//	return false;
	}
	if ( temp_total != total_num )
		return false;
	//if ( cluster_by_block.size() != prior_data.size() )
	//	return false;
	return true;
}

unsigned int cCluster_Info::current_size() const {
	// not thread safe
	unsigned int unique_inventers = 0;
	for ( map < string, cRecGroup >::const_iterator cp = cluster_by_block.begin(); cp != cluster_by_block.end(); ++cp )
		for ( cRecGroup::const_iterator cq = cp->second.begin(); cq != cp ->second.end(); ++ cq )
			//unique_inventers += cq->second.size();
			unique_inventers += cq->get_fellows().size();
	return unique_inventers;
}



void cCluster_Info::retrieve_last_comparision_info ( const cBlocking_Operation & blocker, const char * const past_comparision_file) {
	//bool debug = false;
	try {
		const int firstname_index = cRecord::get_index_by_name(cFirstname::static_get_class_name());
		const int lastname_index = cRecord::get_index_by_name(cLastname::static_get_class_name());

		std::ifstream::sync_with_stdio(false);
		std::ifstream infile(past_comparision_file);
		const unsigned int primary_delim_size = strlen(primary_delim);
		const unsigned int secondary_delim_size = strlen(secondary_delim);
		cGroup_Value empty_set;
		map < string , cRecGroup >::iterator prim_iter;
		map < const string*, map < const cRecord *, double> >::iterator prim_co_iter;
		//unsigned int seq = 0;
		unsigned int count = 0;
		const unsigned int base = 100000;
		cluster_by_block.clear();
		this->firstname_stat.clear();
		this->lastname_stat.clear();
		this->max_occurrence.clear();
		//cohesion_map_by_block.clear();
		if (infile.good()) {
			string filedata;
			cRecGroup::iterator pm;
			while ( getline(infile, filedata)) {
				//++seq;
				register size_t pos = 0, prev_pos = 0;
				pos = filedata.find(primary_delim, prev_pos);
				string keystring = filedata.substr( prev_pos, pos - prev_pos);
				const cRecord * key = retrieve_record_pointer_by_unique_id( keystring, *uid2record_pointer);
				//const string * mk = ref.record2blockingstring.find(key)->second;
				const string b_id = blocker.extract_blocking_info(key);
				const string firstname_part = blocker.extract_column_info(key, firstname_index);
				const string lastname_part = blocker.extract_column_info(key, lastname_index);


				prim_iter = cluster_by_block.find(b_id);

				prev_pos = pos + primary_delim_size;

				pos = filedata.find(primary_delim, prev_pos);
				double val = 0;
				if ( is_matching ) {
					string cohesionstring = filedata.substr( prev_pos, pos - prev_pos);
					val = atof(cohesionstring.c_str());
					/*
					prim_co_iter = cohesion_map_by_block.find(&prim_iter->first);
					if ( prim_co_iter != cohesion_map_by_block.end()) {
						prim_co_iter->second.insert(std::pair<const cRecord*, double>(key, val));
					}
					else {
						map<const cRecord *, double> one_elem;
						one_elem.insert(std::pair<const cRecord*, double>(key, val));
						cohesion_map_by_block.insert(std::pair<const string *, map<const cRecord*, double> >(&(prim_iter->first), one_elem));
					}
					*/
				}
				prev_pos = pos + primary_delim_size;


				cGroup_Value tempv;
				while ( ( pos = filedata.find(secondary_delim, prev_pos) )!= string::npos){
					string valuestring = filedata.substr( prev_pos, pos - prev_pos);
					const cRecord * value = retrieve_record_pointer_by_unique_id( valuestring, *uid2record_pointer);

					//if ( valuestring == string ("04325803-2"))
						//debug = true;
					//pm->second.insert(value);
					//pm->second.push_back(value);
					tempv.push_back(value);
					prev_pos = pos + secondary_delim_size;
				}

				//cGroup_Key tempk (key, seq);
				//cGroup_Key tempk(key);
				cCluster_Head th(key, val);
				cCluster tempc(th, tempv);
				//std::cout << "************" << std::endl;
				//tempc.get_cluster_head().m_delegate->print();

				if ( prim_iter != cluster_by_block.end()) {
					//pm = prim_iter->second.insert(std::pair<cGroup_Key, cGroup_Value >(tempk, empty_set)).first;
					//prim_iter->second.push_back( std::pair<cGroup_Key, cGroup_Value >(tempk, empty_set));
					//push back a cRecGroup = cCluster

					prim_iter->second.push_back(tempc);
					//pm = prim_iter->second.end();
					//--pm;
				}
				else {
					cRecGroup one_elem(1, tempc);
					//one_elem.push_back(std::pair<cGroup_Key, cGroup_Value>(tempk, empty_set));
					//prim_iter = cluster_by_block.insert(std::pair<string, cRecGroup>(b_id, one_elem)).first;
					prim_iter = cluster_by_block.insert(std::pair<string, cRecGroup>(b_id, one_elem)).first;
					//pm = prim_iter->second.begin();
					++ (this->firstname_stat[firstname_part]);
					++ (this->lastname_stat[lastname_part]);

				}

				++count;
				if ( count % base == 0 )
					std::cout << count << " records have been loaded from the cluster file. " << std::endl;
			}

			//std::cout << "Self correcting ...";
			//for ( prim_iter = cluster_by_block.begin(); prim_iter != cluster_by_block.end(); ++ prim_iter ) {
			//	for ( cRecGroup::iterator p = prim_iter->second.begin(); p != prim_iter->second.end(); ++p ) {
			//		p->self_repair();
			//	}
			//}
			//std::cout << "Done." << std::endl;
			std::cout << "Obtained " << firstname_stat.size() << " unique firstname part and " << lastname_stat.size() << " unique lastname part." << std::endl;
			this->firstname_dist.clear();
			this->lastname_dist.clear();
			for ( map<string, unsigned int >::const_iterator p = firstname_stat.begin(); p != firstname_stat.end(); ++p )
				++ ( firstname_dist[p->second]);
			for ( map<string, unsigned int >::const_iterator p = lastname_stat.begin(); p != lastname_stat.end(); ++p )
				++ ( lastname_dist[p->second]);

			unsigned int stat_cnt = 0;
			for ( map < unsigned int , unsigned int >::const_iterator p = firstname_dist.begin(); p != firstname_dist.end(); ++p )
				if ( p->first > stat_cnt )	{
					stat_cnt = p->first;
				}
			for ( map<string, unsigned int >::const_iterator p = firstname_stat.begin(); p != firstname_stat.end(); ++p )
				if ( p->second == stat_cnt )
					std::cout << "Most common firstname part = " << p->first << " Occurrence = " << stat_cnt << std::endl;
			max_occurrence.push_back(stat_cnt);

			stat_cnt = 0;
			for ( map < unsigned int , unsigned int >::const_iterator p = lastname_dist.begin(); p != lastname_dist.end(); ++p )
				if ( p->first > stat_cnt )	{
					stat_cnt = p->first;
				}
			for ( map<string, unsigned int >::const_iterator p = lastname_stat.begin(); p != lastname_stat.end(); ++p )
				if ( p->second == stat_cnt )
					std::cout << "Most common lastname part = " << p->first << " Occurrence = " << stat_cnt << std::endl;
			max_occurrence.push_back(stat_cnt);



			std::ofstream ff("firstname_dist.txt");
			std::ofstream lf ( "lastname_dist.txt");
			const char * temp_delim = ",";
			for ( map < unsigned int , unsigned int >::const_iterator p = firstname_dist.begin(); p != firstname_dist.end(); ++p )
				ff << p->first << temp_delim << p->second << '\n';
			for ( map < unsigned int , unsigned int >::const_iterator p = lastname_dist.begin(); p != lastname_dist.end(); ++p )
				lf << p->first << temp_delim << p->second << '\n';

			std::cout << past_comparision_file << " has been read into memory as "
						<<  ( is_matching ? "MATCHING" : "NON-MATCHING" ) << " reference." << std::endl;
		}
		else {
			throw cException_File_Not_Found(past_comparision_file);
		}
	}
	catch ( const cException_Attribute_Not_In_Tree & except) {
		std::cout << " Current Unique-identifier Binary Tree, having "<< uid2record_pointer->size()
		<< " elements, is not complete! " << std::endl;
		std::cout << past_comparision_file << " has unique identifiers that do not show in the current binary tree. " << std::endl;
		std::cout << except.what() <<  " is missing. ( More unique identifiers are believed to be missing ) ." << std::endl;
		std::cout << "Also need to check whether each row is in the format of <key, primary_delimiter, cohesion_value, primary delimiter, <<value, secondary_delimiter,>>*n >" << std::endl;
		throw;
	}
}

void cCluster_Info::reset_blocking(const cBlocking_Operation & blocker, const char * const past_comparision_file) {

	total_num = 0;
	useless = blocker.get_useless_string();
	retrieve_last_comparision_info(blocker, past_comparision_file);


	config_prior();



	for ( map <string, cRecGroup>::const_iterator p  = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
		for ( cRecGroup::const_iterator cp = p->second.begin(); cp != p->second.end(); ++cp )
			//total_num += cp->second.size();
			total_num += cp->get_fellows().size();
	}
}

void cCluster_Info::preliminary_consolidation(const cBlocking_Operation & blocker, const list < const cRecord *> & all_rec_list) {
	std::cout << "Preliminary consolidation ... ..." << std::endl;
	total_num = 0;
	cluster_by_block.clear();
	useless = blocker.get_useless_string();
	map < string, cRecGroup >::iterator mi;
	const cGroup_Value empty_fellows;
	for ( list < const cRecord * > ::const_iterator p = all_rec_list.begin(); p != all_rec_list.end(); ++p ) {
		string temp ( blocker.extract_blocking_info(*p));
		mi = cluster_by_block.find(temp);
		if ( mi == cluster_by_block.end() ) {
			cCluster_Head th(*p, 1);
			cCluster tc(th, empty_fellows);
			cRecGroup tr(1, tc);
			mi = cluster_by_block.insert(std::pair<string, cRecGroup>(temp, tr)).first;
		}
		mi->second.front().insert_elem(*p);
	}
	for ( mi = cluster_by_block.begin(); mi != cluster_by_block.end(); ++mi ) {
		cCluster & alias = mi->second.front();
		alias.self_repair();
		//std::cout << "**************" << std::endl;
		//for ( cGroup_Value::const_iterator p = alias.get_fellows().begin(); p != alias.get_fellows().end(); ++ p)
		//	(*p)->print();

	}
	std::cout << "Preliminary consolidation done." << std::endl;
	//config_prior();
	for ( map <string, cRecGroup>::const_iterator p  = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
		for ( cRecGroup::const_iterator cp = p->second.begin(); cp != p->second.end(); ++cp )
			//total_num += cp->second.size();
			total_num += cp->get_fellows().size();
	}

}


void cCluster_Info::output_current_comparision_info( const char * const outputfile ) const {
	std::ofstream of (outputfile);
	std::cout << "Dumping to " << outputfile << std::endl;
	print(of);
	std::cout << outputfile << " has been created or updated. "<< std::endl;
}



void cCluster_Info::print(std::ostream & os) const {
	if ( is_matching && (! is_consistent() ))
		throw cException_Duplicate_Attribute_In_Tree("Not Consistent!");
	//if ( is_matching && past_comparision.size() != cohesion_map.size() )
		//throw cException_Cluster_Error(" Size of comparision map and of cohesion map are different.");
	std::ostream::sync_with_stdio(false);
	const string & uid_name = cUnique_Record_ID::static_get_class_name();
	const unsigned int uid_index = cRecord::get_index_by_name(uid_name);
	static const cException_Vector_Data except(uid_name.c_str());
	//map<const string*, map<const cRecord *, double > >::const_iterator pco = cohesion_map_by_block.begin();
	for ( map <string, cRecGroup >::const_iterator q = cluster_by_block.begin(); q != cluster_by_block.end(); ++q ) {
		//if ( pco ->first != &q->first)
			//throw cException_Cluster_Error("Comparision map and cohesion map are not consistent.");
		//map < const cRecord *, double >::const_iterator pcohesion = pco->second.begin();
		//const map < const cRecord *, double > & cohesion_alias = cohesion_map_by_block.find(&q->first)->second;
		for ( cRecGroup::const_iterator p = q->second.begin(); p != q->second.end(); ++p ) {
			//if ( is_matching && p->first.get_precord() != pcohesion->first )
			//	throw cException_Cluster_Error("Comparision map and cohesion map are not consistent.");
			//const cAttribute * key_pattrib = p->first.get_precord()->get_attrib_pointer_by_index(uid_index);
			//const cAttribute * key_pattrib = p->first->get_attrib_pointer_by_index(uid_index);
			const cAttribute * key_pattrib = p->get_cluster_head().m_delegate->get_attrib_pointer_by_index(uid_index);
			if ( key_pattrib->get_data().size() != 1 )
				throw except;
			os << * key_pattrib->get_data().at(0) << primary_delim;

			double cohesion_value;
			if ( is_matching )
				//cohesion_value = pcohesion->second;
				//cohesion_value = cohesion_alias.find(p->first)->second;
				cohesion_value = p->get_cluster_head().m_cohesion;
			else
				cohesion_value = 0;

			os << cohesion_value << primary_delim;

			for ( cGroup_Value::const_iterator q = p->get_fellows().begin(); q != p->get_fellows().end(); ++q ) {
				const cAttribute * value_pattrib = (*q)->get_attrib_pointer_by_index(uid_index);
				if ( value_pattrib->get_data().size() != 1 )
					throw except;
				os << * value_pattrib->get_data().at(0) << secondary_delim;
			}
			os << '\n';

			//if ( is_matching)
			//	++pcohesion;
		}
		//if (is_matching)
		//	++pco;
	}
}


unsigned int cCluster_Info::reset_debug_activity( const char * const filename ) {
	const char * const delim = cCluster_Info::secondary_delim;
	unsigned int cnt = 0;
	std::cout << "Resetting block activity for debug purpose in accordance with file " << filename << " ...  " << std::endl;
	this->debug_activity.clear();
	map < string , cRecGroup >::const_iterator cpm;
	for ( cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++cpm ) {
		debug_activity.insert(std::pair<const string*, bool>(&cpm->first, false));
	}


	std::ifstream infile(filename);
	string data;
	while ( getline(infile, data)) {

		while(true)   {
			size_t  pos(0);
			if (   (pos = data.find(delim)) != string::npos   )
				data.replace(pos, strlen(delim), cBlocking_Operation::delim);
			else
				break;
		}
		cpm = this->cluster_by_block.find(data);
		if ( cpm == cluster_by_block.end())
			std::cout << data << " is not a good block identifier." << std::endl;
		else {
			const string * pstr = & cluster_by_block.find(data)->first;
			debug_activity.find(pstr)->second = true;
			++cnt;
		}
	}

	std::cout << "Done." << std::endl;
	if ( cnt != 0 )
		std::cout << cnt <<  " blocks have been activated." << std::endl;
	else {
		std::cout << "Warning: Since 0 blocks are active, all will be ACTIVATED instead." << std::endl;
		for ( map< const string *, bool>::iterator p = debug_activity.begin(); p != debug_activity.end(); ++p )
			p->second = true;
		cnt = debug_activity.size();
	}
	return cnt;
}


/*
void cCluster_Info:: update_comparision( const cRecord* key_to_add, const set< const cRecord * > & set_to_add,
										const cRecord * key_to_delete, const double cohesion_to_update, const string * bid) {
	comparision_insert(key_to_add, set_to_add, bid);
	cohesion_update(key_to_add, cohesion_to_update, bid);
	comparision_erase(key_to_delete, bid);
	cohesion_erase(key_to_delete, bid);
};

void cCluster_Info:: comparision_insert(const cRecord* key, const cRecord* one_more_value, const string * bid) {
	static const cGroup_Value empty_set;
	map < const string *, map < const cRecord *, cGroup_Value::iterator q = cluster_by_block.find(bid);
	if ( q == cluster_by_block.end())
		throw cException_Attribute_Not_In_Tree(bid->c_str());
	map < const cRecord*, cGroup_Valuesecond.find(key);
	if ( p == q->second.end() )
		p = q->second.insert(std::pair< const cRecord *, cGroup_Value(key, empty_set) ).first;
	p->second.insert(one_more_value);
}
*/

void cCluster_Info::output_list ( list<const cRecord *> & target) const {
	target.clear();
	for ( map < string, cRecGroup >::const_iterator cp = cluster_by_block.begin(); cp != cluster_by_block.end(); ++cp )
		for ( cRecGroup::const_iterator cq = cp->second.begin(); cq != cp ->second.end(); ++ cq )
			//target.push_back(cq->first);
			target.push_back(cq->get_cluster_head().m_delegate);
}

void cCluster_Info::config_prior()  {
	prior_data.clear();
	std::cout << "Creating prior values ..." << std::endl;

#if 0
	const cString_Remain_Same nochange;
	vector <string> cnames;
	cnames.push_back(cFirstname::static_get_class_name());
	cnames.push_back(cLastname::static_get_class_name());
	cnames.push_back(cAssignee::static_get_class_name());
	const vector < const cString_Manipulator *> sm(cnames.size(), &nochange);
	cBlocking_Operation_Multiple_Column_Manipulate prior_blocking(sm, cnames);
#if 0
	unsigned int max_size = 0;
	unsigned int min_size = 100000;
	for ( map<string, cRecGroup >::const_iterator cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++ cpm) {
		max_size = max_val < unsigned int >( max_size, cpm->second.size() );
		min_size = min_val < unsigned int > ( min_size, cpm->second.size() );
	}
	// PrM( max_size) = 0.1, PrM( min_size ) = 0.9
	const double PrM_max = 0.9;
	const double PrM_min = 0.1;

	const double slope = (PrM_min - PrM_max)/(max_size - min_size);
	const double y_intercept = PrM_max - slope * min_size;

	for ( map<string, cRecGroup >::const_iterator cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++ cpm) {
		/*
		double temp_prior = 1 - static_cast<double> (cpm->second.size())/max_size;
		if ( temp_prior > 0.9 )
			temp_prior = 0.9;
		else if ( temp_prior < 0.01 )
			temp_prior = 0.01;
			*/

		double temp_prior = slope * cpm->second.size() + y_intercept;
		prior_data.insert(std::pair<const string*, double> (& (cpm->first), temp_prior));
	}
#endif
	const double prior_max = 0.9;
	const double prior_min = 1e-20;
	for ( map<string, cRecGroup >::const_iterator cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++ cpm) {
		map < string, unsigned int > count_map;
		map < string, unsigned int > ::iterator pcount;
		for ( cRecGroup::const_iterator cp = cpm->second.begin(); cp != cpm->second.end(); ++cp ) {
			const cGroup_Value & alias = cp->get_fellows();
			for ( cGroup_Value::const_iterator g = alias.begin(); g != alias.end(); ++g ) {
				string temp ( prior_blocking.extract_blocking_info(*g));
				pcount = count_map.find(temp);
				if ( pcount == count_map.end())
					count_map.insert(std::pair<string, unsigned int >(temp, 1));
				else
					++ (pcount->second);
			}
		}
		double numerator = 0;
		unsigned int tt = 0;
		for ( map < string , unsigned int >::const_iterator q = count_map.begin(); q != count_map.end(); ++q ) {
			const unsigned int c = q->second;
			numerator += 1.0 * c * ( c - 1 );
			tt += c;
		}
		double denominator = 1.0 * tt * ( tt - 1 );
		if ( denominator == 0 )
			denominator = 1e10;
		double prior = numerator / denominator ;
		if ( prior > prior_max )
			prior = prior_max;
		else if ( prior < prior_min )
			prior = prior_min;

		prior_data.insert(std::pair<const string*, double> (& (cpm->first), prior));

	}
#endif

	for ( map<string, cRecGroup >::const_iterator cpm = cluster_by_block.begin(); cpm != cluster_by_block.end(); ++ cpm) {
		double prior = get_prior_value(cpm->first, cpm->second);

		prior_data.insert(std::pair<const string*, double> (& (cpm->first), prior));

	}

	std::cout << "Prior values map created." << std::endl;
}

double cCluster_Info::get_prior_value( const string & block_identifier, const list <cCluster> & rg ) {
	static const double prior_max = 0.9;
	static const double prior_default = 1e-6;

	double numerator = 0;
	unsigned int tt = 0;
	for ( list<cCluster>::const_iterator q = rg.begin(); q != rg.end(); ++q ) {
		const unsigned int c = q->get_fellows().size();
		numerator += 1.0 * c * ( c - 1 );
		tt += c;
	}
	double denominator = 1.0 * tt * ( tt - 1 );
	if ( denominator == 0 )
		denominator = 1e10;
	double prior = numerator / denominator ;

	//decompose the block_identifier string so as to get the frequency of each piece
	size_t pos = 0, prev_pos = 0;
	pos = block_identifier.find(cBlocking_Operation::delim, prev_pos );
	string fn_piece = block_identifier.substr( prev_pos, pos - prev_pos );
	prev_pos += pos + cBlocking_Operation::delim.size();
	pos = block_identifier.find(cBlocking_Operation::delim, prev_pos );
	string ln_piece = block_identifier.substr( prev_pos, pos - prev_pos );

	const double first_factor = 1.0 + log ( 1.0 * max_occurrence.at(0) / this->firstname_stat[fn_piece]);
	const double last_factor = 1.0 + log ( 1.0 * max_occurrence.at(1) / this->lastname_stat[ln_piece]);

	prior *= first_factor * last_factor;

	if ( prior > prior_max )
		prior = prior_max;
	else if ( prior == 0 )
		prior = prior_default;

	return prior;
}




void cCluster_Info::disambiguate(const cRatios & ratio, const unsigned int num_threads, const char * const debug_block_file) {
	if ( is_matching_cluster() != true )
		throw cException_Cluster_Error("Wrong type of clusters for disambiguation.");
	unsigned int size_to_disambig = this->reset_debug_activity(debug_block_file);

	std::cout << "Starting disambiguation ... ..." << std::endl;
	cRecGroup emptyone;
	const cGroup_Value emptyset;
	map < string , cRecGroup >::iterator pdisambiged;

	// now starting disambiguation.
	// here can be multithreaded.
	// variables to sync: match, nonmatch, prior_iterator, cnt.
	//map < const string*, double, cString_Pointer_Compare > ::const_iterator prior_map_iterator = prior_data.begin();
	std::cout << "There are "<< size_to_disambig << " blocks to disambiguate." << std::endl;
	/*
	unsigned int cnt = 0;
	const unsigned int base = 10000;
	for ( pdisambiged = disambiguated_data.begin(); pdisambiged != disambiguated_data.end(); ++pdisambiged ) {
		if (  prior_map_iterator->first  != pdisambiged->first ||
				match.get_comparision_map().size() != match.get_cohesion_map().size() )
			throw cBlocking::cException_Tree_Key_Mismatch( prior_map_iterator->first->c_str() );
		disambiguate_by_block(pdisambiged->second, match, nonmatch, prior_map_iterator->second, ratiosmap);
		++ prior_map_iterator;
		++cnt;
		//std::cout << cnt << "th block compared."<<std::endl;
	}
	*/
	//ppdisambiged(&input_pdisambiged), match_alias(match), nonmatch_alias(nonmatch), Prior_map_alias(Prior_map), ratiosmap_alias(ratiosmap), Data_map_alias(source_data)
	pdisambiged = cluster_by_block.begin();
	cWorker_For_Disambiguation sample(pdisambiged, ratio, *this);

	//const unsigned int num_threads = 4;
	vector < cWorker_For_Disambiguation > worker_vector( num_threads, sample);

	for ( unsigned int i = 0; i < num_threads; ++i )
		worker_vector.at(i).start();
	for ( unsigned int i = 0; i < num_threads; ++i )
		worker_vector.at(i).join();

	std::cout << "Disambiguation done! " ;
	std::cout << cWorker_For_Disambiguation::get_count() << " blocks were eventually disambiguated." << std::endl;
	cWorker_For_Disambiguation::zero_count();


	unsigned int max_inventor = 0;
	const cCluster * pmax = NULL;
	unsigned int max_coauthor = 0;
	const cCluster * pmc = NULL;
	for ( map < string, cRecGroup >::const_iterator p = cluster_by_block.begin(); p != cluster_by_block.end(); ++p ) {
		const cRecGroup & galias = p->second;
		for ( cRecGroup::const_iterator q = galias.begin(); q != galias.end(); ++q ) {
			const unsigned int t = q->get_fellows().size();
			if ( t > max_inventor ) {
				max_inventor = t;
				pmax = &(*q);
			}
			const unsigned int c = q->get_coauthor_set().size();
			if ( c > max_coauthor ) {
				max_coauthor = c;
				pmc = &(*q);
			}
		}
	}
	const unsigned int fi = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int li = cRecord::get_index_by_name(cLastname::static_get_class_name());
	const unsigned int ui = cRecord::get_index_by_name(cUnique_Record_ID::static_get_class_name());
	std::cout << std::endl;
	std::cout << "Most consolidated cluster: " << * pmax->get_cluster_head().m_delegate->get_data_by_index(fi).at(0)
			<<"." << * pmax->get_cluster_head().m_delegate->get_data_by_index(li).at(0)
			<< "  ID = " << * pmax->get_cluster_head().m_delegate->get_data_by_index(ui).at(0)
			<<". Size = " << max_inventor << std::endl;
	std::cout << "His coauthors: ";
	for (set< const string* >::const_iterator p = pmax->get_coauthor_set().begin(); p != pmax->get_coauthor_set().end(); ++p )
		std::cout << **p << " | ";
	std::cout << std::endl << std::endl;

	std::cout << "Most collaborative inventor: " << * pmc->get_cluster_head().m_delegate->get_data_by_index(fi).at(0)
					<<"." << * pmc->get_cluster_head().m_delegate->get_data_by_index(li).at(0)
					<< "  ID = " << * pmc->get_cluster_head().m_delegate->get_data_by_index(ui).at(0)
					<<". Size = " << max_coauthor << std::endl;
	std::cout << "His coauthors: ";
	for (set< const string* >::const_iterator p = pmc->get_coauthor_set().begin(); p != pmc->get_coauthor_set().end(); ++p )
		std::cout << **p << " | ";
	std::cout << std::endl << std::endl;

	const char * collaborative_file = "Collaborative.txt";
	std::ofstream of(collaborative_file);
	std::cout << "Saving the most collaborative inventers detail in " << collaborative_file << std::endl;
	pmc->get_cluster_head().m_delegate->print(of);
	for ( cGroup_Value::const_iterator p = pmc->get_fellows().begin(); p != pmc->get_fellows().end(); ++p )
		(*p)->print(of);
	std::cout << "Saved." << std::endl;

}


void cWorker_For_Disambiguation::run() {
	try {
		const unsigned int base = 10000;
		map < string, cCluster_Info::cRecGroup >::iterator pthis;
		while ( true) {
			pthread_mutex_lock(&iter_lock);
			if ( *ppdisambiged == cluster_ref.get_cluster_map().end() ) {
				pthread_mutex_unlock(&iter_lock);
				break;
			}
			pthis = *ppdisambiged;
			++(*ppdisambiged);
			pthread_mutex_unlock(&iter_lock);

			bool is_success = disambiguate_wrapper(pthis, cluster_ref, *pratios);
			pthread_mutex_lock(&iter_lock);
			if ( is_success )
				++count;
			if ( count % base == 0 && count != 0 ) {
				std::cout << count << " blocks have been disambiguated." << std::endl;
			}
			pthread_mutex_unlock(&iter_lock);
		}
	}
	catch ( const cAbstract_Exception & ex) {
		std::cout << "Caught user defined error: " << ex.what() << " . --Reported by thread " << getThreadID() << std::endl;
		throw;
	}

	//catch ( ... ) {
	//	std::cout << "Caught UNKNOWN error (USUALLY CRITICAL). Need to handle this! "<< " . --Reported by thread " << getThreadID()  << std::endl;
	//	throw;
	//}
}



bool disambiguate_wrapper(const map<string, cCluster_Info::cRecGroup>::iterator & p,cCluster_Info & cluster,
							const cRatios & ratio ) {
	const string * pst = &p->first;
	if ( p->first == cluster.get_useless_string() ) {
		std::cout << "Block Without Any Infomation Tag: " << p->first << " Size = " << p->second.size() << "----------SKIPPED."<< std::endl;
		return false;
	}
	if ( cluster.debug_activity.find(pst)->second == false )
		return false;

	if ( p->second.size() > 3000)
		std::cout << "Block Very Big: " << p->first << " Size = " << p->second.size() << std::endl;
	//else
	unsigned int temp1 = 0, temp2 = 0;
	const unsigned int max_round = 40;
	for ( unsigned int i = 0; i < max_round; ++i ){
		temp1 = temp2;
		temp2 = cluster.disambiguate_by_block(p->second, cluster.get_prior_map().find(pst)->second, ratio, pst);
		if ( temp2 == temp1 )
			return true;
	}
	std::cout << "============= POSSIBLE FAILURE IN BLOCK ==============" << std::endl;
	std::cout << p->first << " has exceeded max rounds within block disambiguation !!" << std::endl;
	std::cout << "============= END OF WARNING =============" << std::endl;
	return false;
}

unsigned int cCluster_Info:: disambiguate_by_block ( cRecGroup & to_be_disambiged_group,  double & prior_value,
							const cRatios & ratio, const string * const bid ) {
	//const bool debug_mode = true;
	//const bool update_prior = false;
	//divide and conquer
	const unsigned int group_size = to_be_disambiged_group.size();
	if ( group_size == 1 )
		return group_size;
	cRecGroup::iterator split_cursor = to_be_disambiged_group.begin();
	for ( unsigned int i = 0; i < group_size / 2 ; ++i )
		++ split_cursor;
	cRecGroup secondpart ( split_cursor, to_be_disambiged_group.end());
	to_be_disambiged_group.erase( split_cursor, to_be_disambiged_group.end() );

	disambiguate_by_block( to_be_disambiged_group, prior_value, ratio, bid );
	disambiguate_by_block( secondpart, prior_value, ratio, bid );

	// now compare the two disambiguated parts;

	cRecGroup::iterator first_iter, second_iter;

	const double threshold_list[] = {	0.95 };

	for ( unsigned int round = 0; round < sizeof(threshold_list)/sizeof(double); ++ round) {

		for ( first_iter = to_be_disambiged_group.begin(); first_iter != to_be_disambiged_group.end(); ) {
			bool should_increment_first = true;
			//double first_cohesion = match.find_cohesion( first_iter->first.get_precord() )->second;
			//double first_cohesion = match.find_cohesion( first_iter->first.get_precord(), bid)->second;
			//map < const cRecord *, double >  & cohesion_alias = this->get_cohesion_map(bid);
			//double & first_cohesion = cohesion_alias.find(first_iter->first.get_precord())->second;
			//double first_cohesion = cohesion_alias.find(first_iter->first)->second;
			//double first_cohesion = first_iter->get_cluster_head().m_cohesion;
			for ( second_iter = secondpart.begin(); second_iter != secondpart.end(); ) {
				bool should_increment_second = true;
				//const double second_cohesion = match.find_cohesion( second_iter->first.get_precord(), bid )->second;
				//double second_cohesion = cohesion_alias.find(second_iter->first)->second;
				//double second_cohesion = second_iter->get_cluster_head().m_cohesion;

				//std::pair<const cRecord *, double> result =  disambiguate_by_set(first_iter->first, first_iter->second,
				//		first_cohesion, second_iter->first, second_iter->second, second_cohesion, prior_value, ratio, 0 );

				cCluster_Head result = first_iter->disambiguate(*second_iter, prior_value, threshold_list[round]);
				if ( debug_mode && result.m_delegate != NULL) {
					std::cout << "**************************" << std::endl;
					first_iter->get_cluster_head().m_delegate->print(std::cout);
					second_iter->get_cluster_head().m_delegate->print(std::cout);
					std::cout << "Prior value = "<< prior_value << " Probability = " << result.m_cohesion << std::endl;
					std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl << std::endl;
				}
				// check result now.
				// if result != NULL, the pointer is the representative of the merged group;
				// the two sets should merge in the disambiguated_data map.
				// matched cluster should be modified to accomodate the merge of the two sets.
				// and a merged score should also be added to the cluster.


				if ( result.m_delegate != NULL ) {
					//now merge in the disambiguated_data.
					if ( result.m_delegate == second_iter->get_cluster_head().m_delegate ) {
						/*
						// merge outer to inner
						second_iter->second.insert(second_iter->second.end(), first_iter->second.begin(), first_iter->second.end() );
						// modifying clusters.
						cohesion_alias.find(second_iter->first)->second = result.second;
						second_cohesion = result.second;
						cohesion_alias.erase(first_iter->first);
						*/

						second_iter->merge(*first_iter, result);

						to_be_disambiged_group.erase(first_iter++);
						should_increment_first = false;
						break;
					}
					/*
					else if ( result.m_delegate == first_iter->get_cluster_head().m_delegate){
						// merge inner to outer;
						first_iter->merge(*second_iter, result);

						//first_iter->second.insert(first_iter->second.end(), second_iter->second.begin(), second_iter->second.end() );
						// modify cluster
						//cohesion_alias.erase(second_iter->first);
						secondpart.erase( second_iter++ );
						should_increment_second = false;
						//cohesion_alias.find(first_iter->first)->second = result.second;
						//first_cohesion = result.m_cohesion;
					}
					*/
					else {
						// merge inner to outer and change the outer key
						first_iter->merge(*second_iter, result);
						//first_iter->second.insert(first_iter->second.end(), second_iter->second.begin(), second_iter->second.end() );
						//const cRecord * key_to_delete = first_iter->first;
						//cGroup_Key & firstkey_to_change = const_cast< cGroup_Key& > ( first_iter->first);
						//firstkey_to_change = result.first ;
						//cohesion_alias.erase(key_to_delete);
						//cohesion_alias.erase(second_iter->first);
						//cohesion_alias.insert(std::pair<const cRecord *, double>(result.first, result.second));
						secondpart.erase( second_iter++ );
						should_increment_second = false;
						//first_cohesion = result.m_cohesion;
					}
				}
				// the two sets should not merge.
				// simply add information to the nonmatch cluster
				else {
					//nonmatch.comparision_insert(first_iter->first.get_precord(), second_iter->first.get_precord());
					//nonmatch.comparision_insert(second_iter->first.get_precord(), first_iter->first.get_precord());
				}
				if ( should_increment_second)
					++ second_iter;
			}
			if ( should_increment_first)
				++ first_iter;
		}
	}
		// now merge the two.

	to_be_disambiged_group.insert(to_be_disambiged_group.end(), secondpart.begin(), secondpart.end());
	//const double new_prior = get_prior_value( * bid, to_be_disambiged_group);
	//if ( autoupdate_prior_mode )
	//	prior_value = new_prior;

	return to_be_disambiged_group.size();

}


void cCluster_Info::disambig_assignee( const char * outputfile) const {
	std::ofstream ofmerge("Merging.txt");
	typedef std::pair<const cRecord *, const cRecord *> ptrpair;
	typedef set < const cRecord *, cSort_by_attrib > asgset;
	//typedef list < const cRecord *> asgset;
	typedef std::pair < const cRecord *, asgset > asgdict;

	const double threshold = 0.90;
	const unsigned int patent_num_threshold = 10;
	list < ptrpair > one2one_correction;
	const unsigned int asgindex = cRecord::get_index_by_name( cAssignee::static_get_class_name() );
	const unsigned int appyearindex = cRecord::get_index_by_name( cApplyYear::static_get_class_name());

	cSort_by_attrib asg_sorter(cAssignee::static_get_class_name());

	map < const cRecord*, unsigned int , cSort_by_attrib > asgmap(asg_sorter);
	map < const cRecord*, unsigned int , cSort_by_attrib >::iterator pasg;

	for ( map < string, cRecGroup>::const_iterator pm = cluster_by_block.begin(); pm != cluster_by_block.end(); ++pm ){
		for ( cRecGroup::const_iterator pgroup = pm->second.begin(); pgroup != pm->second.end(); ++pgroup ) {

			//for ( cGroup_Value :: const_iterator p = pgroup->second.begin(); p != pgroup->second.end(); ++p ) {
			for ( cGroup_Value :: const_iterator p = pgroup->get_fellows().begin(); p != pgroup->get_fellows().end(); ++p ) {
				//const string & asg_alias = (*p)->get_data_by_index(asgindex).at(0);
				pasg = asgmap.find ( *p );
				if ( pasg != asgmap.end())
					++ (pasg->second);
				else
					asgmap.insert(std::pair< const cRecord *, unsigned int>(*p,1));
			}
		}
	}
	std::cout << "There are " << asgmap.size() << " unique assignees." << std::endl;

	map < const cRecord *, const cRecord*, cSort_by_attrib> tracer(asg_sorter);
	map < const cRecord *, const cRecord*, cSort_by_attrib>::iterator ptracer, qtracer;
	const cRecord * key, * value;
	for ( map < string, cRecGroup>::const_iterator pm = cluster_by_block.begin(); pm != cluster_by_block.end(); ++pm ){
		for ( cRecGroup::const_iterator pgroup = pm->second.begin(); pgroup != pm->second.end(); ++pgroup ) {


			//for ( cGroup_Value :: const_iterator p = pgroup->second.begin(); p != pgroup->second.end(); ++p ) {
			for ( cGroup_Value :: const_iterator p = pgroup->get_fellows().begin(); p != pgroup->get_fellows().end(); ++p ) {
				const string & pst = * (*p)->get_data_by_index(asgindex).at(0);

				cGroup_Value::const_iterator q = p;
				for ( ; q != pgroup->get_fellows().end(); ++q ) {
					const string & qst = * (*q)->get_data_by_index(asgindex).at(0);
					if ( (*p)->get_data_by_index(appyearindex).at(0) == (*q)->get_data_by_index(appyearindex).at(0)
							&& ( ! (*p)->get_data_by_index(appyearindex).at(0)->empty() )
						 && strcmp95_modified( pst.c_str(), qst.c_str()) > threshold ) {
						map < const cRecord*, unsigned int , cSort_by_attrib >::const_iterator pp = asgmap.find( *p );
						map < const cRecord*, unsigned int , cSort_by_attrib >::const_iterator qq = asgmap.find( *q );

						if ( pp -> second >= qq->second ) {
							key = *q;
							value = *p;
						}
						else {
							key = *p;
							value = *q;
						}


						if ( tracer.find(key) == tracer.end() ) {
							tracer.insert(ptrpair(key, value));
						}
						ptracer = tracer.find(key);

						if ( tracer.find(value) == tracer.end() )
							tracer.insert(ptrpair(value,value));
						qtracer = tracer.find(value);


						//the most confusing part
						// if the key and the value are not equivalent, keep searching.
						while ( asg_sorter( ptracer->first, ptracer->second) || asg_sorter(ptracer->second, ptracer->first) )
							ptracer = tracer.find(ptracer->second);
						// compare the ptracer->second and the value, and determine which will be the final value;


						while ( asg_sorter( qtracer->first, qtracer->second) || asg_sorter(qtracer->second, qtracer->first))
							qtracer = tracer.find( qtracer->second);

						if ( asgmap.find(ptracer->second) -> second > patent_num_threshold && asgmap.find(qtracer->second )->second > patent_num_threshold )
							continue;
						if ( asgmap.find(ptracer->second)->second < asgmap.find(qtracer->second)->second ) {
							//if ( tracer.find(value) == tracer.end() )
								//tracer.insert(ptrpair(value, value));
							ptracer->second = qtracer->second;
						}
						else {
							qtracer->second = ptracer->second;
						}

					}
				}
			}

		}
	}

	//now build dictionaries.
	map < const cRecord *, asgset, cSort_by_attrib> dict(asg_sorter);
	map < const cRecord *, asgset, cSort_by_attrib>::iterator pdict;

	map < const cRecord *, const cRecord*, cSort_by_attrib>::const_iterator cpt1, cpt2;
	for (  cpt1 = tracer.begin(); cpt1 != tracer.end(); ++cpt1 ) {
		cpt2 = cpt1;
		while ( asg_sorter( cpt2->first, cpt2->second) || asg_sorter(cpt2->second, cpt2->first) )
			cpt2 = tracer.find(cpt2->second);
		pdict = dict.find(cpt2->second);
		if ( pdict == dict.end() ) {
			asgset temp(asg_sorter);
			//temp.push_back(cpt2->second);
			//temp.push_back(cpt1->first);
			temp.insert(cpt2->second);
			temp.insert(cpt1->first);
			dict.insert(std::pair<const cRecord*, asgset>(cpt2->second, temp));
		}
		else {
			//pdict->second.push_back(cpt1->first);
			pdict->second.insert(cpt1->first);
		}
	}

	//now ready to output
	unsigned int count = 0;
	std::ios::sync_with_stdio(false);
	std::ofstream of (outputfile);
	for ( map < const cRecord *, asgset, cSort_by_attrib>::const_iterator p = dict.begin(); p != dict.end(); ++p ) {
		const string & k = * p->first->get_data_by_index(asgindex).at(0);
		of << k;
		of << cCluster_Info::primary_delim;
		of << 0;
		of << cCluster_Info::primary_delim;
		for ( asgset::const_iterator q = p->second.begin(); q != p->second.end(); ++q ) {
			const string & v = * (*q)->get_data_by_index(asgindex).at(0);
			of << v << cCluster_Info::secondary_delim;
			++count;
		}
		of << '\n';
	}

	std::ios::sync_with_stdio(true);
	std:: cout << "Assignee disambiguation gives " << count << " unique assignees." << std::endl;

}


