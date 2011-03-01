/*
 * DisambigEngine.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: ysun
 */

#include "DisambigEngine.h"
#include "DisambigDefs.h"
#include "DisambigFileOper.h"
#include "DisambigCluster.h"
#include "DisambigRatios.h"
#include "DisambigNewCluster.h"
//#include "cstdio"

#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>


using std::map;
using std::set;
vector <string> cRecord::column_names;

unsigned int cWorker_For_Disambiguation::count = 0;
pthread_mutex_t cWorker_For_Disambiguation::iter_lock = PTHREAD_MUTEX_INITIALIZER;
const string cBlocking_Operation::delim = "##";


void clear_records(const list <cRecord> & source) {
	for (list<cRecord>::const_iterator p = source.begin(); p != source.end(); ++p) {
		for (vector <const cAttribute * >::const_iterator q = p->vector_pdata.begin(); q != p->vector_pdata.end(); ++q )
			delete *q; //*q is the pointer
	}
}


unsigned int cRecord::informative_attributes() const {

	static const unsigned int firstname_index = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	static const unsigned int middlename_index = cRecord::get_index_by_name(cMiddlename::static_get_class_name());
	static const unsigned int lastname_index = cRecord::get_index_by_name(cLastname::static_get_class_name());
	static const unsigned int assignee_index = cRecord::get_index_by_name(cAssignee::static_get_class_name());
	static const unsigned int class_index = cRecord::get_index_by_name(cClass::static_get_class_name());


	unsigned int cnt = 0;

	this->vector_pdata.at(firstname_index)->is_informative() && (++cnt);
	this->vector_pdata.at(middlename_index)->is_informative() && (++cnt);
	this->vector_pdata.at(lastname_index)->is_informative() && (++cnt);
	this->vector_pdata.at(assignee_index)->is_informative() && (++cnt);
	this->vector_pdata.at(class_index)->is_informative() && (++cnt);

#if 0
	for ( vector <const cAttribute *>::const_iterator p = vector_pdata.begin(); p != vector_pdata.end(); ++p  ) {
		if ( (*p)->is_comparator_activated() ) {
			if ( (*p)->is_informative() )
				++cnt;
		}
	}
#endif
	return cnt;
}



void cRecord::print( std::ostream & os ) const {
	const char lend = '\n';
	for ( vector <const cAttribute *>::const_iterator p = this->vector_pdata.begin(); p != this->vector_pdata.end(); ++p )
		(*p)->print( os );
	os << "===============================" << lend;
}

vector <unsigned int> cRecord::record_compare(const cRecord & rhs) const {
	vector <unsigned int > rec_comp_result;
	try{
		for ( unsigned int i = 0; i < this->vector_pdata.size(); ++i ) {
			try {
				unsigned int stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
				rec_comp_result.push_back( stage_result );
			}
			catch (const cException_No_Comparision_Function & err) {
				//std::cout << err.what() << " does not have comparision function. " << std::endl;
			}
		}
	}
	catch ( const cException_Interactive_Misalignment & except) {
		std::cout << "Skipped" << std::endl;
		rec_comp_result.clear();
	}
	return rec_comp_result;
}


vector <unsigned int> cRecord::record_compare_by_attrib_indice (const cRecord &rhs, 
																const vector < unsigned int > & attrib_indice_to_compare) const {
	vector <unsigned int > rec_comp_result;
	try{
		for ( unsigned int j = 0; j < attrib_indice_to_compare.size(); ++j ) {
			try {
				unsigned int i = attrib_indice_to_compare.at(j);
				unsigned int stage_result = this->vector_pdata[i]->compare(*(rhs.vector_pdata[i]));
				rec_comp_result.push_back( stage_result );
			}
			catch (const cException_No_Comparision_Function & err) {
				//std::cout << err.what() << " does not have comparision function. " << std::endl;
			}
		}
	}
	catch ( const cException_Interactive_Misalignment & except) {
		std::cout << "Skipped" << std::endl;
		rec_comp_result.clear();
	}
	return rec_comp_result;
}


unsigned int cRecord::exact_compare(const cRecord & rhs ) const {
	unsigned int result = 0;
	for ( unsigned int i = 0; i < this->vector_pdata.size(); ++i ) {
		int ans = this->vector_pdata.at(i)->exact_compare( * rhs.vector_pdata.at(i));
		if ( 1 == ans )
			++result;
	}
	return result;
}

void cRecord::print() const { this->print(std::cout);}

unsigned int cRecord::get_index_by_name(const string & inputstr) {
	for ( unsigned int i = 0 ; i < column_names.size(); ++i )
		if ( column_names.at(i) == inputstr )
			return i;
	throw cException_ColumnName_Not_Found(inputstr.c_str());
}

inline string cString_Truncate::manipulate( const string & inputstring ) const {
	if ( ! is_usable )
		throw cException_Blocking_Disabled("String Truncation not activated yet.");

	if ( 0 == nchar ) {
		if ( is_forward )
			return inputstring;
		else {
			string output = inputstring;
			std::reverse(output.begin(), output.end());
			return output;
		}
	}

	if ( inputstring.size() == 0 )
		return inputstring;

	char * p = new char[ nchar + 1];
	const char * res = p;
	const char * source;
	if ( begin >= 0 && static_cast<unsigned int >(begin) < inputstring.size() )
		source = &inputstring.at(begin);
	else if ( begin < 0 && ( begin + inputstring.size() >= 0 ) )
		source = &inputstring.at( begin + inputstring.size() );
	else {
		delete [] p;
		throw cString_Truncate::cException_String_Truncation(inputstring.c_str());
	}

	if ( is_forward) {
		for ( unsigned int i = 0; i < nchar && *source !='\0'; ++i )
			*p++ = *source++;
		*p = '\0';
	}
	else {
		for ( unsigned int i = 0; i < nchar && source != inputstring.c_str() ; ++i )
			*p++ = *source--;
		*p = '\0';
	}
	string result (res);
	delete [] res;
	return result;
}

string cExtract_Initials::manipulate( const string & inputstring ) const {
	size_t pos, prev_pos;
	pos = prev_pos = 0;
	
	if ( inputstring.empty() )
		return string();
	list < char > tempres;
	do {		
		tempres.push_back(inputstring.at(prev_pos) );
		pos = inputstring.find(delimiter, prev_pos );
		prev_pos = pos + 1;
	} while ( pos != string::npos );
	
	
	const unsigned int word_count = tempres.size();
	if ( word_count >= starting_word ) {
		for ( unsigned int i = 0; i < starting_word; ++i )
			tempres.pop_front();
	}
	return string(tempres.begin(), tempres.end());
}

string cString_Extract_FirstWord::manipulate( const string & inputstring ) const {
	string res = inputstring.substr(0, inputstring.find(delimiter, 0));
	return res;
}


cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(const list < const cRecord * > & all_rec_pointers,
																	const cCluster_Info & cluster, const unsigned int coauthors)
	: patent_tree(cSort_by_attrib(cPatent::static_get_class_name())), num_coauthors(coauthors) {
	if ( num_coauthors > 4 ) {
		std::cout << "================ WARNING =====================" << std::endl;
		std::cout << "Number of coauthors in which cBlocking_Operation_By_Coauthors uses is probably too large. Number of coauthors = " << num_coauthors << std::endl;
		std::cout << "==================END OF WARNING ================" << std::endl;
	}
	build_patent_tree(all_rec_pointers);
	build_uid2uinv_tree(cluster);
	for ( unsigned int i = 0; i < num_coauthors; ++i ) {
		infoless += cBlocking_Operation::delim;
		infoless += cBlocking_Operation::delim;
	}
}

cBlocking_Operation_By_Coauthors::cBlocking_Operation_By_Coauthors(const list < const cRecord * > & all_rec_pointers, const unsigned int coauthors)
	: patent_tree(cSort_by_attrib(cPatent::static_get_class_name())), num_coauthors(coauthors) {
	if ( num_coauthors > 4 ) {
		std::cout << "================ WARNING =====================" << std::endl;
		std::cout << "Number of coauthors in which cBlocking_Operation_By_Coauthors uses is probably too large. Number of coauthors = " << num_coauthors << std::endl;
		std::cout << "==================END OF WARNING ================" << std::endl;
	}
	build_patent_tree(all_rec_pointers);
	for ( unsigned int i = 0; i < num_coauthors; ++i ) {
		infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
		infoless += cBlocking_Operation::delim + cBlocking_Operation::delim;
	}
}



void cBlocking_Operation_By_Coauthors::build_patent_tree(const list < const cRecord * > & all_rec_pointers) {
	map < const cRecord *, cGroup_Value, cSort_by_attrib >::iterator ppatentmap;
	for ( list < const cRecord * >::const_iterator p = all_rec_pointers.begin(); p != all_rec_pointers.end(); ++p ) {
		ppatentmap = patent_tree.find(*p);
		if ( ppatentmap == patent_tree.end() ) {
			cGroup_Value temp ( 1, *p);
			patent_tree.insert( std::pair < const cRecord *, cGroup_Value > (*p, temp) );
		}
		else {
			ppatentmap->second.push_back(*p);
		}
	}
}

void cBlocking_Operation_By_Coauthors::build_uid2uinv_tree( const cCluster_Info & cluster )  {
	uinv2count_tree.clear();
	uid2uinv_tree.clear();
	unsigned int count = 0;
	typedef list<cCluster> cRecGroup;
	std::cout << "Building trees: 1. Unique Record ID to Unique Inventer ID. 2 Unique Inventer ID to Number of holding patents ........" << std::endl;
	//for ( map < string, cRecGroup >::const_iterator p = cluster.get_cluster_map().begin(); p != cluster.get_cluster_map().end(); ++p ) {
	for ( map < string, cRecGroup >::const_iterator p = cluster.get_cluster_map().begin(); p != cluster.get_cluster_map().end(); ++p ) {
		for ( cRecGroup::const_iterator q = p->second.begin(); q != p->second.end(); ++q ) {
			const cRecord * value = q->get_cluster_head().m_delegate;
			map < const cRecord *, unsigned int >::iterator pcount = uinv2count_tree.find(value);
			if ( pcount == uinv2count_tree.end() )
				pcount = uinv2count_tree.insert(std::pair<const cRecord *, unsigned int>(value, 0)).first;

			for ( cGroup_Value::const_iterator r = q->get_fellows().begin(); r != q->get_fellows().end(); ++r ) {
				const cRecord * key = *r;
				uid2uinv_tree.insert(std::pair< const cRecord * , const cRecord *> (key, value ));
				++ ( pcount->second);
				++count;
			}
		}
	}
	std::cout << count << " nodes has been created inside the tree." << std::endl;
}


cGroup_Value cBlocking_Operation_By_Coauthors::get_topN_coauthors( const cRecord * prec, const unsigned int topN ) const {
	const cGroup_Value & list_alias = patent_tree.find(prec)->second;
	map < unsigned int, cGroup_Value > occurrence_map;
	unsigned int cnt = 0;
	for ( cGroup_Value::const_iterator p = list_alias.begin(); p != list_alias.end(); ++p ) {
		if ( *p == prec )
			continue;

		map < const cRecord *, const cRecord * >::const_iterator puid2uiv = uid2uinv_tree.find(*p);
		if ( puid2uiv == uid2uinv_tree.end() )
			throw cException_Other("Critical Error: unique record id to unique inventer id tree is incomplete!!");
		const cRecord * coauthor_pointer = puid2uiv->second;

		map < const cRecord *, unsigned int >::const_iterator puinv2count = uinv2count_tree.find(coauthor_pointer);
		if ( puinv2count == uinv2count_tree.end())
			throw cException_Other("Critical Error: unique inventer id to number of holding patents tree is incomplete!!");
		const unsigned int coauthor_count = puinv2count->second;

		if ( cnt <= topN || coauthor_count > occurrence_map.begin() ->first ) {
			map < unsigned int, cGroup_Value >::iterator poccur = occurrence_map.find ( coauthor_count );
			if ( poccur == occurrence_map.end() ) {
				cGroup_Value temp (1, coauthor_pointer);
				occurrence_map.insert(std::pair<unsigned int, cGroup_Value>(coauthor_count, temp));
			}
			else
				poccur->second.push_back(coauthor_pointer);
			//occurrence_map.insert(std::pair< unsigned int, const cRecord *>(coauthor_count, coauthor_pointer));

			if ( cnt < topN )
				++cnt;
			else {
				map < unsigned int, cGroup_Value >::iterator pbegin = occurrence_map.begin();
				pbegin->second.pop_back();
				if ( pbegin->second.empty() )
					occurrence_map.erase(pbegin);
			}
		}
	}
	//output
	cGroup_Value ans;
	for ( map < unsigned int, cGroup_Value>::const_reverse_iterator rp = occurrence_map.rbegin(); rp != occurrence_map.rend(); ++rp )
		ans.insert(ans.end(), rp->second.begin(), rp->second.end());
	return ans;

}

string cBlocking_Operation_By_Coauthors::extract_blocking_info(const cRecord * prec) const {
	const cGroup_Value top_coauthor_list = get_topN_coauthors(prec, num_coauthors);
	// now make string
	const unsigned int firstnameindex = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int lastnameindex = cRecord::get_index_by_name(cLastname::static_get_class_name());

	string answer;
	for ( cGroup_Value::const_iterator p = top_coauthor_list.begin(); p != top_coauthor_list.end(); ++p ) {
		answer += *(*p)->get_data_by_index(firstnameindex).at(0);
		answer += cBlocking_Operation::delim;
		answer += *(*p)->get_data_by_index(lastnameindex).at(0);
		answer += cBlocking_Operation::delim;
	}
	if ( answer.empty() )
		answer = infoless;
	return answer;
}


void cReconfigurator_AsianNames::reconfigure( const cRecord * p ) const {
	bool need_reconfigure = false;
	const string & country = *  p->get_attrib_pointer_by_index(country_index)->get_data().at(0) ;
	for ( register vector<string>::const_iterator ci = east_asian.begin(); ci != east_asian.end(); ++ci )
		if ( country == *ci ) {
			need_reconfigure = true;
			break;
		}

	if ( need_reconfigure == false )
		return;

	// do not change original attributes. add new ones.
	const string & fn_alias = * p->get_attrib_pointer_by_index(firstname_index)->get_data().at(0);
	const vector <string> fn ( 2, fn_alias );
	const cAttribute * paf = cFirstname::static_clone_by_data(fn);

	const string & lnstr = * p->get_attrib_pointer_by_index(lastname_index)->get_data().at(0);
	const string mnstr ( fn_alias + "." + lnstr);
	const vector < string > mn(2, mnstr);
	const cAttribute * pam = cMiddlename ::static_clone_by_data(mn);

	cRecord * q = const_cast < cRecord * > (p);
	q->set_attrib_pointer_by_index(paf, firstname_index);
	q->set_attrib_pointer_by_index(pam, middlename_index);


#if 0
	const string & fullname = mn.at(0);
	static const char delim = ' ';
	size_t pos = fullname.find(delim);
	if ( pos != string::npos ) {
		string midname = fullname.substr(pos);
		vector <string > & mnc = const_cast< vector<string> & > (mn);
		string altered = this->rmvsp.manipulate(midname);
		mnc.at(1) = altered;
	}
#endif
	//p->print(std::cout);
}


void cReconfigurator_Latitude_Interactives::reconfigure ( const cRecord * p ) const {
	//watch out sequence here !!!
	if ( ! cLatitude::is_enabled() )
		return;

	vector < const cAttribute * > interact;
	interact.push_back(p->get_attrib_pointer_by_index(this->longitude_index));
	interact.push_back(p->get_attrib_pointer_by_index(this->street_index));
	interact.push_back(p->get_attrib_pointer_by_index(this->country_index));
	const cAttribute * tp = p->get_attrib_pointer_by_index(this->latitude_index);
	cAttribute * lp = const_cast<cAttribute *> (tp);
	lp->reset_interactive(interact);
}






void cRecord::reset_coauthors(const cBlocking_Operation_By_Coauthors & bb, const unsigned int topN) const {
	const unsigned int firstname_attrib_index = cRecord::get_index_by_name(cFirstname::static_get_class_name());
	const unsigned int lastname_attrib_index = cRecord::get_index_by_name(cLastname::static_get_class_name());
	const unsigned int coauthor_attrib_index = cRecord::get_index_by_name(cCoauthor::static_get_class_name());
	const cAttribute * pAttrib = this->get_attrib_pointer_by_index(coauthor_attrib_index);
	const cCoauthor * pCoauthorAttrib = dynamic_cast< const cCoauthor *> (pAttrib);
	if ( pCoauthorAttrib == NULL )
		throw cException_Other("Critical Error: Attribute is not Coauthor !");

	if ( bb.patent_tree.find (this) == bb.patent_tree.end() )
		throw cException_Other("Critical Error: Record not found !");

	const cGroup_Value top_coauthor_list = bb.get_topN_coauthors(this, topN);
	if ( top_coauthor_list.empty() )
		return;

	//now edit cCoauthor.
	cCoauthor * pCoauthor = const_cast < cCoauthor *> (pCoauthorAttrib);
	string coauthor_str;
	const string primary_delim = "/";
	const string secondary_delim = ".";
	cGroup_Value::const_iterator p = top_coauthor_list.begin();
	coauthor_str += * (*p)->get_data_by_index(firstname_attrib_index).at(0);
	coauthor_str += secondary_delim;
	coauthor_str += * (*p)->get_data_by_index(lastname_attrib_index).at(0);
	++p;
	for ( ; p != top_coauthor_list.end(); ++p ) {
		coauthor_str += primary_delim;
		coauthor_str += * (*p)->get_data_by_index(firstname_attrib_index).at(0);
		coauthor_str += secondary_delim;
		coauthor_str += * (*p)->get_data_by_index(lastname_attrib_index).at(0);
	}

	pCoauthor->reset_data(coauthor_str.c_str());

}









cBlocking::cBlocking (const list<const cRecord *> & psource,
						const vector<string> & blocking_column_names,
						const vector<const cString_Manipulator*>& pmanipulators,
						const string & unique_identifier)
					: blocking_column_names(blocking_column_names), string_manipulator_pointers(pmanipulators){

	const string label_delim = cBlocking_Operation::delim;
	const unsigned int num_block_columns = blocking_column_names.size();
	if ( num_block_columns != pmanipulators.size())
		throw cBlocking::cException_Blocking("Blocking Constructor Error.");

	cSort_by_attrib unique_comparator(unique_identifier);
	vector < unsigned int > blocking_indice;
	for ( unsigned int i = 0; i < num_block_columns; ++i ) {
		blocking_indice.push_back(cRecord::get_index_by_name(blocking_column_names.at(i)));
	}

	map<string, cGroup_Value >::iterator b_iter;
	const map<string, cGroup_Value >::key_compare bmap_compare = blocking_data.key_comp();
	std::cout << "Grouping ..." << std::endl;
	unsigned int count = 0;
	const unsigned int base = 100000;
	for ( list<const cRecord*>::const_iterator p = psource.begin(); p != psource.end(); ++p ) {
		string label;
		for ( unsigned int i = 0; i < num_block_columns; ++i ) {
			const vector < const string * > & source_data = (*p)->get_data_by_index(blocking_indice.at(i));
			if ( source_data.size() == 0 )
				throw cException_Vector_Data( (*p)->get_column_names().at(blocking_indice.at(i)).c_str());
			//for ( vector <string> :: const_iterator q = source_data.begin(); q != source_data.end(); ++q ) {
			vector < const string* > :: const_iterator q = source_data.begin();
			label += pmanipulators.at(i)->manipulate(**q);
			label += label_delim;
			//}
		}

		b_iter = blocking_data.lower_bound(label);
		if ( b_iter != blocking_data.end() && ! bmap_compare(label, b_iter->first) ) {
			//b_iter->second.insert(*p);
			b_iter->second.push_back(*p);
		}
		else {
			cGroup_Value tempset;
			//tempset.insert( *p );
			tempset.push_back(*p);
			b_iter = blocking_data.insert(b_iter, std::pair< string, cGroup_Value >(label, tempset));
		}

		record2blockingstring.insert(std::pair<const cRecord*, const string *>(*p, &(b_iter->first)));
		++count;
		if ( count % base == 0 )
			std::cout << count << " records have been grouped into blocks." << std::endl;
	}
}

#if 0
cBlocking_For_Disambiguation::cBlocking_For_Disambiguation (const list<const cRecord*> & psource,
		const vector<string> & blocking_column_names, const vector<const cString_Manipulator*>& pmanipulators,
		const string & unique_identifier )
		: cBlocking ( psource, blocking_column_names, pmanipulators, unique_identifier ) {
	config_prior();
};


void cBlocking_For_Disambiguation::config_prior() {
	unsigned int max_size = 0;
	unsigned int min_size = 100000;
	for ( map<string, set<const cRecord*> >::const_iterator cpm = blocking_data.begin(); cpm != blocking_data.end(); ++ cpm) {
		max_size = max_val < unsigned int >( max_size, cpm->second.size() );
		min_size = min_val < unsigned int > ( min_size, cpm->second.size() );
	}
	// PrM( max_size) = 0.1, PrM( min_size ) = 0.9
	const double PrM_max = 0.9;
	const double PrM_min = 0.1;

	const double slope = (PrM_min - PrM_max)/(max_size - min_size);
	const double y_intercept = PrM_max - slope * min_size;

	for ( map<string, set<const cRecord*> >::const_iterator cpm = blocking_data.begin(); cpm != blocking_data.end(); ++ cpm) {
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
}



void cBlocking_For_Disambiguation::disambiguate(cCluster_Info & match, cCluster_Info & nonmatch,
												const map < vector < unsigned int >, double, cSimilarity_Compare > & ratiosmap) {
	if ( match.is_matching_cluster() != true || nonmatch.is_matching_cluster() != false )
		throw cException_Blocking("Wrong type of clusters for disambiguation.");
	

	map < const string *, cRecGroup, cBlocking::cString_Pointer_Compare > disambiguated_data;
	cRecGroup emptyone;
	const cGroup_Value emptyset;
	map < const string *, cRecGroup, cBlocking::cString_Pointer_Compare >::iterator pdisambiged;
	
	//preparing data from previous disambiguation . 
	for ( map<string, set<const cRecord *> >::iterator pmap = blocking_data.begin(); pmap != blocking_data.end(); ++pmap ) {
		const string * block_id = &(pmap->first);
		const map < const cRecord *, cGroup_Value & match_map = match.get_comparision_map(block_id);
		unsigned int sequence = 0;
		pdisambiged = disambiguated_data.insert(std::pair< const string*, cRecGroup> (&(pmap->first), emptyone ) ).first;
		
		map < const cRecord *, cGroup_Value :: const_iterator q;
		for ( set< const cRecord *>::const_iterator p = pmap->second.begin(); p != pmap->second.end(); ++ p ) {
			cGroup_Key tempkey(*p, sequence++);
			q = match.find_comparision(*p, block_id);
			if ( q != match_map.end() ) {
				pdisambiged->second.insert(std::pair< cGroup_Key, cGroup_Valuesecond ) );
			}
			else {
				pdisambiged->second.insert(std::pair< cGroup_Key, cGroup_Value(tempkey, emptyset ) );
			}
		}
	}
	// now starting disambiguation.
	// here can be multithreaded.
	// variables to sync: match, nonmatch, prior_iterator, cnt.
	map < const string*, double, cString_Pointer_Compare > ::const_iterator prior_map_iterator = prior_data.begin();
	std::cout << "There are "<<disambiguated_data.size() << " blocks to disambiguate." << std::endl;
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
	pdisambiged = disambiguated_data.begin();
	cWorker_For_Disambiguation sample(pdisambiged, match, nonmatch, prior_data, ratiosmap, disambiguated_data);

	const unsigned int num_threads = 4;
	vector < cWorker_For_Disambiguation > worker_vector( num_threads, sample);

	for ( unsigned int i = 0; i < num_threads; ++i )
		worker_vector.at(i).start();
	for ( unsigned int i = 0; i < num_threads; ++i )
		worker_vector.at(i).join();



}



void disambiguate_by_block ( cBlocking_For_Disambiguation::cRecGroup & to_be_disambiged_group, 
							cCluster_Info & match, cCluster_Info & nonmatch, const double prior_value,
							const map < vector < unsigned int >, double, cSimilarity_Compare > & ratiosmap, const string * const bid ) {
	//divide and conquer
	const unsigned int group_size = to_be_disambiged_group.size();
	if ( group_size == 1 )
		return;
	map < cBlocking_For_Disambiguation::cGroup_Key, cGroup_Value::iterator split_cursor = to_be_disambiged_group.begin();
	for ( unsigned int i = 0; i < group_size / 2 ; ++i )
		++ split_cursor;
	map < cBlocking_For_Disambiguation::cGroup_Key, cGroup_Value secondpart ( split_cursor, to_be_disambiged_group.end());
	to_be_disambiged_group.erase( split_cursor, to_be_disambiged_group.end() );
	
	disambiguate_by_block( to_be_disambiged_group, match, nonmatch, prior_value, ratiosmap, bid );
	disambiguate_by_block( secondpart, match, nonmatch, prior_value, ratiosmap, bid );
	
	// now compare the two disambiguated parts;
	
	map < cBlocking_For_Disambiguation::cGroup_Key, cGroup_Value::iterator first_iter, second_iter;
	
	for ( first_iter = to_be_disambiged_group.begin(); first_iter != to_be_disambiged_group.end(); ) {
		bool should_increment_first = true;
		//double first_cohesion = match.find_cohesion( first_iter->first.get_precord() )->second;
		double first_cohesion = match.find_cohesion( first_iter->first.get_precord(), bid)->second;
		for ( second_iter = secondpart.begin(); second_iter != secondpart.end(); ) {
			bool should_increment_second = true;
			const double second_cohesion = match.find_cohesion( second_iter->first.get_precord(), bid )->second;

			std::pair<const cRecord *, double> result =  disambiguate_by_set(first_iter->first.get_precord(), first_iter->second, 
					first_cohesion, second_iter->first.get_precord(), second_iter->second, second_cohesion, prior_value, ratiosmap);
			// check result now.
			// if result != NULL, the pointer is the representative of the merged group;
			// the two sets should merge in the disambiguated_data map.
			// matched cluster should be modified to accomodate the merge of the two sets.
			// and a merged score should also be added to the cluster.
			if ( result.first != NULL ) {
				//now merge in the disambiguated_data.
				if ( result.first == second_iter->first.get_precord() ) {
					// merge outer to inner
					second_iter->second.insert(first_iter->second.begin(), first_iter->second.end() );
					// modifying clusters.
					match.update_comparision(result.first, first_iter->second,
											 first_iter->first.get_precord(), result.second, bid);
					to_be_disambiged_group.erase(first_iter++);
					should_increment_first = false;
					break;
				}
				else if ( result.first == first_iter->first.get_precord() ){
					// merge inner to outer;
					first_iter->second.insert( second_iter->second.begin(), second_iter->second.end() );
					// modify cluster
					match.update_comparision(result.first, second_iter->second,
											 second_iter->first.get_precord(), result.second, bid);
					secondpart.erase( second_iter++ );
					should_increment_second = false;
					first_cohesion = result.second;
				}
				else {
					// merge inner to outer and change the outer key
					first_iter->second.insert( second_iter->second.begin(), second_iter->second.end() );
					const cRecord * key_to_delete = first_iter->first.get_precord();
					cBlocking_For_Disambiguation:: cGroup_Key & firstkey_to_change = const_cast< cBlocking_For_Disambiguation:: cGroup_Key& > ( first_iter->first);
					firstkey_to_change.set_precord( result.first );
					match.update_comparision(result.first, second_iter->second,
											 second_iter->first.get_precord(), result.second, bid);
					match.update_comparision(result.first, first_iter->second,
											 key_to_delete, result.second, bid);
					secondpart.erase( second_iter++ );
					should_increment_second = false;
					first_cohesion = result.second;
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
	
	// now merge the two.
	
	to_be_disambiged_group.insert(secondpart.begin(), secondpart.end());
	
}

#endif

double fetch_ratio(const vector < unsigned int > & ratio_to_lookup, const map < vector  < unsigned int>, double, cSimilarity_Compare > & ratiosmap ) {
	map < vector < unsigned int >, double, cSimilarity_Compare >::const_iterator p = ratiosmap.find( ratio_to_lookup);
	if ( p == ratiosmap.end())
		return 0;
	else
		return p->second;
}

/*
double fetch_ratio(const vector < unsigned int > & ratio_to_calc, const vector< double > & coeffs, const unsigned int power ) {
	const unsigned int sz = ratio_to_calc.size();
	if ( sz != coeffs.size() - 1 )
		throw cException_Other("Critical Error in calculating ratios: size of similarity profile != size of coefficients - 1 ");
	double lnr = 0;
	unsigned int i;
	for ( i = 0; i < sz; ++i )
		lnr += coeffs.at(i) * ratio_to_calc.at(i);
	//HERE THE SIGN SHOULD BE IN ACCORDANCE WITH THE GET_COEFFICIENT SUBROUTINE;
	lnr += coeffs.at(i);
	//return exp(lnr);
	return pow(lnr, power);
}
*/




std::pair<const cRecord *, double> disambiguate_by_set (
									const cRecord * key1, const cGroup_Value & match1, const double cohesion1,
									 const cRecord * key2, const cGroup_Value & match2, const double cohesion2,
									 const double prior,
									 const cRatios & ratio,  const double mutual_threshold ) {
	const double minimum_threshold = 0.8;
	const double threshold = max_val <double> (minimum_threshold, mutual_threshold * cohesion1 * cohesion2);
	static const cException_Unknown_Similarity_Profile except(" Fatal Error in Disambig by set.");
	const unsigned int size_minimum = 50;
	double r_value = 0;

	if ( match1.size() > size_minimum && match2.size() > size_minimum ) {
		vector< unsigned int > sp = key1->record_compare(*key2);
		//if ( power == 0 )
			r_value = fetch_ratio(sp, ratio.get_ratios_map());
		//else
		//	throw cException_Other("Error in disambiguate_by_set: invalid input parameter.");
			//r_value = fetch_ratio(sp, ratio.get_coefficients_vector(), power);

		if ( 0 == r_value ) {
			std::cout << " OK. Disabled now but should be enabled in the real run." << std::endl;
			return std::pair<const cRecord *, double> (NULL, 0);
			//throw except;
		}
		const double probability = 1 / ( 1 + ( 1 - prior )/prior / r_value );
		if ( probability < threshold )
			return std::pair<const cRecord *, double> (NULL, probability);
		else
			return std::pair<const cRecord *, double> ( ( ( match1.size() > match2.size() ) ? key1 : key2 ), probability );
	}
	else {
		vector < const cRecord *> merged;
		cGroup_Value::const_iterator q = match1.begin();
		if ( key1 != NULL)
			merged.push_back(key1);
		for ( unsigned int i = 0; i < size_minimum && i < match1.size(); ++i ) {
			if (*q != key1 )
				merged.push_back(*q++);
		}
		q = match2.begin();
		if ( key2 != NULL )
			merged.push_back(key2);
		for ( unsigned int i = 0; i < size_minimum && i < match2.size(); ++i ) {
			if (*q != key2)
				merged.push_back(*q++);
		}

		//const bool should_adjust = ( merged.size() > size_minimum );

		double max_sum = 0;
		double matrix_sum = 0;
		unsigned int chosen_i = 0;
		unsigned int crude_chosen_i = 0;
		const unsigned int size = merged.size();
		unsigned int num_useful_columns = 0;
		for (unsigned int i = 0; i < size; ++i ) {
			unsigned int informative_columns = merged[i]->informative_attributes();
			if ( informative_columns > num_useful_columns )
				num_useful_columns = informative_columns;
		}

		bool is_updated = false;
		vector <unsigned int> max_history;

		for (unsigned int i = 0; i < size; ++i ) {
			double temp_sum = 0;
			unsigned int informative_columns = merged[i]->informative_attributes();

			for (unsigned int j = 0; j < size; ++j ) {
				if ( i == j )
					continue;

				vector< unsigned int > tempsp = merged[i]->record_compare(* merged[j]);
				//if ( power == 0 )
					r_value = fetch_ratio(tempsp, ratio.get_ratios_map());
				//else
					//r_value = fetch_ratio(tempsp, ratio.get_coefficients_vector(), power);

				if ( r_value == 0 ) {
					/*
					std::cout << "Missing Similarity Profile : ";
					for ( vector < unsigned int >::const_iterator qq = tempsp.begin(); qq != tempsp.end(); ++qq )
						std::cout << *qq << ", ";
					std::cout << std::endl;
					*/
					//throw except;
					temp_sum += 0;
				}
				else {
					temp_sum +=  1 / ( 1 + ( 1 - prior )/prior / r_value );
				}
			}

			if ( temp_sum > max_sum ) {
				if ( informative_columns == num_useful_columns ) {
					max_sum = temp_sum;
					chosen_i = i;
					is_updated = true;
				}
				crude_chosen_i = i;
				max_history.clear();
			}

			if ( temp_sum == max_sum ) {
				max_history.push_back(i);
			}

			matrix_sum += temp_sum;
		}

		vector<bool> shall_use(max_history.size(), true);

		const double probability = matrix_sum / size / (size - 1 );
		if ( probability > 1 )
			throw cException_Invalid_Probability("Cohesion value error.");
		if ( probability > threshold ) {
			//double check chosen_i;
			if ( is_updated ) {
				for ( unsigned int k = 0; k != max_history.size(); ++k) {
					const unsigned int seq = max_history.at(k);
					double another_probability = 0;
					for ( unsigned int i = 0; i < size; ++i ) {
						if ( i == seq )
							continue;
						vector< unsigned int > tempsp = merged[i]->record_compare(* merged[seq]);
						r_value = fetch_ratio(tempsp, ratio.get_ratios_map());
						if ( r_value == 0 )
							another_probability += 0;
						else
							another_probability +=  1 / ( 1 + ( 1 - prior )/prior / r_value );
					}
					another_probability /= ( size - 1 ) ;
					if ( another_probability < threshold )
						shall_use.at(k) = false;
				}
			}
			//compare each chosen i s
			is_updated = false;
			for ( vector<bool>::const_iterator bi= shall_use.begin(); bi != shall_use.end(); ++bi )
				if ( *bi == true ) {
					is_updated = true;
					break;
				}

			if ( is_updated ) {
				unsigned int max_exact_match = 0;
				for ( unsigned int i = 0; i < max_history.size(); ++i ) {
					if ( shall_use.at(i) == false )
						continue;

					unsigned int temp_c = 0;
					for ( vector < const cRecord *>::const_iterator p = merged.begin(); p != merged.end(); ++p ) {
						temp_c += merged.at( max_history.at(i) )->exact_compare( **p );
					}
					if ( temp_c > max_exact_match ) {
						max_exact_match = temp_c;
						chosen_i = max_history.at(i);
					}
				}
			}

			const unsigned int ok = ( is_updated ) ? chosen_i : crude_chosen_i;
			return std::pair<const cRecord *, double>( merged[ok], probability );
		}
		else
			return std::pair<const cRecord *, double> (NULL, probability);
	}
}



//==================================================================================================
//There are probably some bugs in this function. Use old one as an expediency.
std::pair<const cRecord *, double> disambiguate_by_set_test (
									const cRecord * key1, const cGroup_Value & match1, const double cohesion1,
									 const cRecord * key2, const cGroup_Value & match2, const double cohesion2,
									 const double prior, 
									 const cRatios & ratio,  const double mutual_threshold ) {
	const double minimum_threshold = 0.8;
	const double threshold = max_val <double> (minimum_threshold, mutual_threshold * cohesion1 * cohesion2);
	static const cException_Unknown_Similarity_Profile except(" Fatal Error in Disambig by set.");
	const unsigned int size_minimum = 50;
	double r_value = 0;

	if ( match1.size() > size_minimum && match2.size() > size_minimum ) {
		vector< unsigned int > sp = key1->record_compare(*key2);
		//if ( power == 0 )
			r_value = fetch_ratio(sp, ratio.get_ratios_map());
		//else
		//	throw cException_Other("Error in disambiguate_by_set: invalid input parameter.");
			//r_value = fetch_ratio(sp, ratio.get_coefficients_vector(), power);

		if ( 0 == r_value ) {
			std::cout << " OK. Disabled now but should be enabled in the real run." << std::endl;
			return std::pair<const cRecord *, double> (NULL, 0);
			//throw except;
		}
		const double probability = 1 / ( 1 + ( 1 - prior )/prior / r_value );
		if ( probability < threshold )
			return std::pair<const cRecord *, double> (NULL, probability);
		else 
			return std::pair<const cRecord *, double> ( ( ( match1.size() > match2.size() ) ? key1 : key2 ), probability );
	}
	else {
		vector < const cRecord *> merged;
		cGroup_Value::const_iterator q = match1.begin();
		if ( key1 != NULL)
			merged.push_back(key1);
		for ( unsigned int i = 0; i < size_minimum && i < match1.size(); ++i ) {
			if (*q != key1 )
				merged.push_back(*q++);
		}
		q = match2.begin();
		if ( key2 != NULL )
			merged.push_back(key2);
		for ( unsigned int i = 0; i < size_minimum && i < match2.size(); ++i ) {
			if (*q != key2)
				merged.push_back(*q++);
		}

		//const bool should_adjust = ( merged.size() > size_minimum );
		
		double max_sum = 0;
		double matrix_sum = 0;
		unsigned int chosen_i = 0;
		//unsigned int crude_chosen_i = 0;
		const unsigned int size = merged.size();
		unsigned int num_useful_columns = 0;
		vector < unsigned int > qualified_index;
		for (unsigned int i = 0; i < size; ++i ) {
			unsigned int informative_columns = merged[i]->informative_attributes();
			if ( informative_columns > num_useful_columns ) {
				num_useful_columns = informative_columns;
				qualified_index.clear();
			}
			if ( informative_columns == num_useful_columns )
				qualified_index.push_back(i);
		}

		vector < double > max_history( qualified_index.size(), 0);
		//bool is_updated = false;


		vector < unsigned int > :: iterator cvi = qualified_index.begin();
		unsigned int location = 0;
		for (unsigned int i = 0; i < size; ++i ) {
		//for ( vector < unsigned int >:: const_iterator cvi = qualified_index.begin(); cvi != qualified_index.end(); ++cvi ) {
			//const unsigned int i = *cvi;
			double temp_sum = 0;
			//unsigned int informative_columns = merged[i]->informative_attributes();

			for (unsigned int j = 0; j < size; ++j ) {
				if ( i == j )
					continue;

				vector< unsigned int > tempsp = merged[i]->record_compare(* merged[j]);
				r_value = fetch_ratio(tempsp, ratio.get_ratios_map());

				if ( r_value == 0 ) {
					temp_sum += 0;
				}
				else {
					temp_sum +=  1 / ( 1 + ( 1 - prior )/prior / r_value );
				}
			}

			if ( temp_sum > max_sum  ) {
				//cvi = find ( qualified_index.begin(), qualified_index.end(), i );
				if ( *cvi == i  ) {
					//qualified_index.erase( cvi );
					max_sum = temp_sum;
					max_history.at(location++) = max_sum;
					++cvi;
				}
			}

			//if ( temp_sum == max_sum ) {
			//	max_history.push_back(i);
			//}

			matrix_sum += temp_sum;
		}

		vector<bool> shall_use(max_history.size(), true);

		const double probability = matrix_sum / size / (size - 1 ); 
		if ( probability > 1 )
			throw cException_Invalid_Probability("Cohesion value error.");
		if ( probability < threshold )
			return std::pair<const cRecord *, double> (NULL, probability);
		else {
			//double check chosen_i;
			location = 0;
			vector < unsigned int > selected;
			for ( unsigned int k = 0; k != max_history.size(); ++k ) {
				if ( max_history.at(k) == max_sum )
					selected.push_back(qualified_index.at(k));
			}
			unsigned int max_exact_count = 0;
			for ( unsigned int k = 0; k != selected.size(); ++k ) {
				unsigned int temp_c = 0;
				for ( vector < const cRecord *>::const_iterator p = merged.begin(); p != merged.end(); ++p ) {
					int ex_comp = merged.at( max_history.at(k) )->exact_compare( **p );
					if ( ex_comp == 1 )
						temp_c += 1;
				}
				if ( temp_c > max_exact_count ) {
					max_exact_count = temp_c;
					chosen_i = k;
				}
			}

			const unsigned int ok = selected.at(chosen_i);
			return std::pair<const cRecord *, double>( merged.at(ok), probability );
		}
	}
}














#if 0
template < typename container >
std::pair<const cRecord *, double> disambiguate_by_set (
									const cRecord * key1, const container & match1, const double cohesion1,
									 const cRecord * key2, const container & match2, const double cohesion2,
									 const double prior,
									 const map < vector < unsigned int >, double, cSimilarity_Compare > & ratiosmap ) {
	const double minimum_threshold = 0.5;
	const double threshold = max_val <double> (minimum_threshold, 0.95 * cohesion1 * cohesion2);
	static const cException_Unknown_Similarity_Profile except(" Fatal Error in Disambig by set.");
	const unsigned int size_minimum = 10;

	if ( match1.size() > size_minimum && match2.size() > size_minimum ) {
		vector< unsigned int > sp = key1->record_compare(*key2);
		map < vector < unsigned int >, double, cSimilarity_Compare >::const_iterator p = ratiosmap.find( sp );
		if ( p == ratiosmap.end() ) {
			std::cout << " OK. Disabled now but should be enabled in the real run." << std::endl;
			return std::pair<const cRecord *, double> (NULL, 0);
			//throw except;
		}
		const double probability = 1 / ( 1 + ( 1 - prior )/prior / p->second );
		if ( probability < threshold )
			return std::pair<const cRecord *, double> (NULL, probability);
		else
			return std::pair<const cRecord *, double> ( ( ( match1.size() > match2.size() ) ? key1 : key2 ), probability );
	}
	else {
		vector < const cRecord *> merged;
		typename container::const_iterator q = match1.begin();
		merged.push_back(key1);
		for ( unsigned int i = 0; i < size_minimum && i < match1.size(); ++i ) {
			merged.push_back(*q++);
		}
		q = match2.begin();
		merged.push_back(key2);
		for ( unsigned int i = 0; i < size_minimum && i < match2.size(); ++i ) {
			merged.push_back(*q++);
		}

		double max_sum = 0;
		double matrix_sum = 0;
		unsigned int chosen_i = 0;
		const unsigned int size = merged.size();
		for (unsigned int i = 0; i < size; ++i ) {
			double temp_sum = 0;
			for (unsigned int j = 0; j < size; ++j ) {
				if ( i == j )
					continue;
				vector< unsigned int > tempsp = merged[i]->record_compare(* merged[j]);
				map < vector < unsigned int >, double, cSimilarity_Compare >::const_iterator p = ratiosmap.find(tempsp);
				if ( p == ratiosmap.end() ) {
					/*
					std::cout << "Missing Similarity Profile : ";
					for ( vector < unsigned int >::const_iterator qq = tempsp.begin(); qq != tempsp.end(); ++qq )
						std::cout << *qq << ", ";
					std::cout << std::endl;
					*/
					//throw except;
					temp_sum += 0;
				}
				else {
					temp_sum +=  1 / ( 1 + ( 1 - prior )/prior / p->second );
				}
			}
			if ( temp_sum > max_sum ) {
				max_sum = temp_sum;
				chosen_i = i;
			}
			matrix_sum += temp_sum;
		}
		const double probability = matrix_sum / size / (size - 1 );
		if ( probability > 1 )
			throw cException_Invalid_Probability("Cohesion value error.");
		if ( probability > threshold )
			return std::pair<const cRecord *, double>( merged[chosen_i], probability );
		else
			return std::pair<const cRecord *, double> (NULL, probability);
	}
}
#endif





// create a binary tree of string (unique id) -> cRecord * in order to retrieve records by unique_id






//======== read txt instead of sqlite3
#if 0
bool fetch_records_from_txt_obsolete(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns, const map<string, asgdetail>& asgtree){
	std::ifstream::sync_with_stdio(false);
	const char * delim = ",";	// this deliminator should never occur in the data.
	const unsigned int delim_size = strlen(delim);
	std::ifstream infile(txt_file);
	if ( ! infile.good()) {
		throw cException_File_Not_Found(txt_file);
	}
	string filedata;
	//getline(infile, filedata);
	//if ( filedata != raw_txt_authenticator )
	//	throw cException_File_Not_Found("Specified file is not a valid one.");

	vector <string> total_col_names;
	getline(infile, filedata);
	register size_t pos, prev_pos;
	pos = prev_pos = 0;
	while (  pos != string::npos){
		pos = filedata.find(delim, prev_pos);
		string columnname;
		if ( pos != string::npos )
			columnname = filedata.substr( prev_pos, pos - prev_pos);
		else
			columnname = filedata.substr( prev_pos );
		total_col_names.push_back(columnname);
		prev_pos = pos + delim_size;
	}
	const unsigned int num_cols = requested_columns.size();
	vector < unsigned int > requested_column_indice;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
		unsigned int j;
		for (  j = 0; j < total_col_names.size(); ++j ) {
			if ( requested_columns.at(i) == total_col_names.at(j) ) {
				requested_column_indice.push_back(j);
				break;
			}
		}
		if ( j == total_col_names.size() ) {
			std::cerr << "Critical Error in reading " << txt_file << std::endl
						<<"Column names not available in the first line. Please Check the correctness." << std::endl;
			throw cException_ColumnName_Not_Found(requested_columns.at(i).c_str());
		}
	}

	cRecord::column_names = requested_columns;
	cAttribute ** pointer_array = new cAttribute *[num_cols];
	
	pos = prev_pos = 0;
	unsigned int position_in_ratios = 0;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
		if ( cRecord::column_names[i] == cFirstname::class_name ) {
			cFirstname::enable();
			pointer_array[i] = new cFirstname();
			cFirstname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLastname::class_name ) {
			cLastname::enable();
			pointer_array[i] = new cLastname();
			cLastname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cMiddlename::class_name ) {
			cMiddlename::enable();
			pointer_array[i] = new cMiddlename();
			cMiddlename::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLatitude::class_name ) {
			cLatitude::enable();
			pointer_array[i] = new cLatitude();
			cLatitude::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLongitude::class_name ) {
			cLongitude::enable();
			pointer_array[i] = new cLongitude();
			cLongitude::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cStreet::class_name ) {
			cStreet::enable();
			pointer_array[i] = new cStreet();
			cStreet::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cCountry::class_name ) {
			cCountry::enable();
			pointer_array[i] = new cCountry();
			cCountry::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cClass::class_name ) {
			cClass::enable();
			pointer_array[i] = new cClass();
			cClass::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cCoauthor::class_name ) {
			cCoauthor::enable();
			pointer_array[i] = new cCoauthor();
			cCoauthor::column_index_in_query = i;
			cCluster::set_coauthor_index(i);
		}
		else if ( cRecord::column_names[i] == cAssignee::class_name ) {
			cAssignee::enable();
			pointer_array[i] = new cAssignee();
			cAssignee::column_index_in_query = i;
			cAssignee::set_assignee_tree_pointer (asgtree);
		}
		else if ( cRecord::column_names[i] == cAsgNum::class_name ) {
			cAsgNum::enable();
			pointer_array[i] = new cAsgNum();
			cAsgNum::column_index_in_query = i;
			cAssignee::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cUnique_Record_ID::class_name ) {
			cUnique_Record_ID::enable();
			pointer_array[i] = new cUnique_Record_ID();
			cUnique_Record_ID::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cApplyYear::class_name ) {
			cApplyYear::enable();
			pointer_array[i] = new cApplyYear();
			cApplyYear::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cCity::class_name ) {
			cCity::enable();
			pointer_array[i] = new cCity();
			cCity::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cPatent::class_name ) {
			cPatent::enable();
			pointer_array[i] = new cPatent();
			cPatent::column_index_in_query = i;
		}
		else {
			for ( unsigned int j = 0; j < i; ++j )
				delete pointer_array[j];
			delete [] pointer_array;
			throw cException_ColumnName_Not_Found(cRecord::column_names[i].c_str());
		}

		if ( pointer_array[i]->get_attrib_group() != string("None") )
			++position_in_ratios;
	}

	// always do this for all the attribute classes
	for ( unsigned int i = 0; i < num_cols; ++i ) 
		pointer_array[i]->check_interactive_consistency(cRecord::column_names);
	//cLatitude::check_interactive_consistency(cRecord::column_names);	// always do this for all the attribute classes.

	std::cout << "Involved attributes are: ";
	for ( unsigned int i = 0; i < num_cols; ++i )
		std::cout << pointer_array[i]->get_class_name() << ", ";
	std::cout << std::endl;

	//vector <char *> string_cache(num_cols );
	vector <string> string_cache(num_cols);
	const unsigned int string_cache_size = 2000;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
	//	string_cache.at(i) = new char [string_cache_size];
		string_cache.at(i).reserve(string_cache_size);
	}

	unsigned long size = 0;
	std::cout << "Reading " << txt_file << " ......"<< std::endl;

	const unsigned int base  =  100000;
	const cAttribute * pAttrib;
	vector <const cAttribute *> temp_vec_attrib;
	vector <const cAttribute *> Latitude_interactive_attribute_pointers;
	while (getline(infile, filedata) ) {
		temp_vec_attrib.clear();
		
		
		for ( unsigned int i = 0; i < num_cols ; ++i ) {
			unsigned int column_location = 0;
			pos = prev_pos = 0;
			while ( column_location++ != requested_column_indice.at(i) ) {
				pos = filedata.find(delim, prev_pos);
				prev_pos = pos + delim_size;
			}
			pos = filedata.find(delim, prev_pos);
			if ( pos == string::npos ) {
				if ( prev_pos != filedata.size() )
					//strcpy( string_cache[i], &filedata.at(prev_pos) );
					string_cache[i] = filedata.substr(prev_pos);
				else
					//strcpy( string_cache[i], "");
					string_cache[i] = "";
			}
			else {
				//char * p_end = std::copy(&filedata.at(prev_pos),&filedata.at(pos), string_cache[i]);
				//*p_end = '\0';
				string_cache[i] = filedata.substr(prev_pos, pos - prev_pos);
			}
			
			pointer_array[i]->reset_data(string_cache[i].c_str());
			pAttrib = pointer_array[i]->clone();	//HERE CREATED NEW CLASS INSTANCES.
			temp_vec_attrib.push_back(pAttrib);
		}
		//now do some reset correlation vector work
		if ( cLatitude::is_enabled() ) {
			Latitude_interactive_attribute_pointers.clear();
			for (vector <unsigned int>::const_iterator pp = cLatitude::interactive_column_indice_in_query.begin(); pp != cLatitude::interactive_column_indice_in_query.end(); ++pp )
				Latitude_interactive_attribute_pointers.push_back(temp_vec_attrib.at(*pp));
			cAttribute * changesomething = const_cast<cAttribute*> (temp_vec_attrib.at(cLatitude::column_index_in_query));
			changesomething->reset_interactive( Latitude_interactive_attribute_pointers );
		}
		// end of correlation reset

		cRecord temprec(temp_vec_attrib);
		source.push_back(temprec);
		//temprec.print();
		++size;
		if ( size % base == 0 )
			std::cout << size << " records obtained." << std::endl;
	}
	std::cout << std::endl;
	std::cout << size << " records have been fetched from "<< txt_file << std::endl;

	//for ( vector <char *> ::iterator str_it = string_cache.begin(); str_it != string_cache.end(); ++ str_it )
		//delete [] (*str_it);
	
	for ( unsigned int i = 0; i < num_cols; ++i )
		delete pointer_array[i];
	delete [] pointer_array;

	return true;
}
#endif


void copyfile(const char * target, const char * source) {
	std::cout << "Copying file " << source << " to " << target << std::endl;
	std::ifstream   input( source,std::ios::binary);
	std::ofstream   output( target,std::ios::binary);

	output   <<   input.rdbuf();

	std::cout << "File copy done." << std::endl;
}



//==========================================
//===========================================

bool fetch_records_from_txt(list <cRecord> & source, const char * txt_file, const vector<string> &requested_columns, const map<string, asgdetail>& asgtree){
	std::ifstream::sync_with_stdio(false);
	const char * delim = ",";	// this deliminator should never occur in the data.
	const unsigned int delim_size = strlen(delim);
	std::ifstream infile(txt_file);
	if ( ! infile.good()) {
		throw cException_File_Not_Found(txt_file);
	}
	string filedata;
	//getline(infile, filedata);
	//if ( filedata != raw_txt_authenticator )
	//	throw cException_File_Not_Found("Specified file is not a valid one.");

	vector <string> total_col_names;
	getline(infile, filedata);
	register size_t pos, prev_pos;
	pos = prev_pos = 0;
	while (  pos != string::npos){
		pos = filedata.find(delim, prev_pos);
		string columnname;
		if ( pos != string::npos )
			columnname = filedata.substr( prev_pos, pos - prev_pos);
		else
			columnname = filedata.substr( prev_pos );
		total_col_names.push_back(columnname);
		prev_pos = pos + delim_size;
	}
	const unsigned int num_cols = requested_columns.size();
	vector < unsigned int > requested_column_indice;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
		unsigned int j;
		for (  j = 0; j < total_col_names.size(); ++j ) {
			if ( requested_columns.at(i) == total_col_names.at(j) ) {
				requested_column_indice.push_back(j);
				break;
			}
		}
		if ( j == total_col_names.size() ) {
			std::cerr << "Critical Error in reading " << txt_file << std::endl
						<<"Column names not available in the first line. Please Check the correctness." << std::endl;
			throw cException_ColumnName_Not_Found(requested_columns.at(i).c_str());
		}
	}

	cRecord::column_names = requested_columns;
	cAttribute ** pointer_array = new cAttribute *[num_cols];

	pos = prev_pos = 0;
	unsigned int position_in_ratios = 0;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
		if ( cRecord::column_names[i] == cFirstname::class_name ) {
			cFirstname::enable();
			pointer_array[i] = new cFirstname();
			cFirstname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLastname::class_name ) {
			cLastname::enable();
			pointer_array[i] = new cLastname();
			cLastname::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cMiddlename::class_name ) {
			cMiddlename::enable();
			pointer_array[i] = new cMiddlename();
			cMiddlename::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLatitude::class_name ) {
			cLatitude::enable();
			pointer_array[i] = new cLatitude();
			cLatitude::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cLongitude::class_name ) {
			cLongitude::enable();
			pointer_array[i] = new cLongitude();
			cLongitude::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cStreet::class_name ) {
			cStreet::enable();
			pointer_array[i] = new cStreet();
			cStreet::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cCountry::class_name ) {
			cCountry::enable();
			pointer_array[i] = new cCountry();
			cCountry::column_index_in_query = i;
			cLatitude::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cClass::class_name ) {
			cClass::enable();
			pointer_array[i] = new cClass();
			cClass::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cCoauthor::class_name ) {
			cCoauthor::enable();
			pointer_array[i] = new cCoauthor();
			cCoauthor::column_index_in_query = i;
			cCluster::set_coauthor_index(i);
		}
		else if ( cRecord::column_names[i] == cAssignee::class_name ) {
			cAssignee::enable();
			pointer_array[i] = new cAssignee();
			cAssignee::column_index_in_query = i;
			cAssignee::set_assignee_tree_pointer (asgtree);
		}
		else if ( cRecord::column_names[i] == cAsgNum::class_name ) {
			cAsgNum::enable();
			pointer_array[i] = new cAsgNum();
			cAsgNum::column_index_in_query = i;
			cAssignee::interactive_column_indice_in_query.push_back(i);
		}
		else if ( cRecord::column_names[i] == cUnique_Record_ID::class_name ) {
			cUnique_Record_ID::enable();
			pointer_array[i] = new cUnique_Record_ID();
			cUnique_Record_ID::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cApplyYear::class_name ) {
			cApplyYear::enable();
			pointer_array[i] = new cApplyYear();
			cApplyYear::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cCity::class_name ) {
			cCity::enable();
			pointer_array[i] = new cCity();
			cCity::column_index_in_query = i;
		}
		else if ( cRecord::column_names[i] == cPatent::class_name ) {
			cPatent::enable();
			pointer_array[i] = new cPatent();
			cPatent::column_index_in_query = i;
		}
		else {
			for ( unsigned int j = 0; j < i; ++j )
				delete pointer_array[j];
			delete [] pointer_array;
			throw cException_ColumnName_Not_Found(cRecord::column_names[i].c_str());
		}

		if ( pointer_array[i]->get_attrib_group() != string("None") )
			++position_in_ratios;
	}

	// always do this for all the attribute classes
	for ( unsigned int i = 0; i < num_cols; ++i )
		pointer_array[i]->check_interactive_consistency(cRecord::column_names);
	//cLatitude::check_interactive_consistency(cRecord::column_names);	// always do this for all the attribute classes.

	std::cout << "Involved attributes are: ";
	for ( unsigned int i = 0; i < num_cols; ++i )
		std::cout << pointer_array[i]->get_class_name() << ", ";
	std::cout << std::endl;

	//vector <char *> string_cache(num_cols );
	vector <string> string_cache(num_cols);
	const unsigned int string_cache_size = 2000;
	for ( unsigned int i = 0; i < num_cols; ++i ) {
	//	string_cache.at(i) = new char [string_cache_size];
		string_cache.at(i).reserve(string_cache_size);
	}

	unsigned long size = 0;
	std::cout << "Reading " << txt_file << " ......"<< std::endl;

	const unsigned int base  =  100000;
	const cAttribute * pAttrib;
	vector <const cAttribute *> temp_vec_attrib;
	vector <const cAttribute *> Latitude_interactive_attribute_pointers;
	while (getline(infile, filedata) ) {
		temp_vec_attrib.clear();


		for ( unsigned int i = 0; i < num_cols ; ++i ) {
			unsigned int column_location = 0;
			pos = prev_pos = 0;
			while ( column_location++ != requested_column_indice.at(i) ) {
				pos = filedata.find(delim, prev_pos);
				prev_pos = pos + delim_size;
			}
			pos = filedata.find(delim, prev_pos);
			if ( pos == string::npos ) {
				if ( prev_pos != filedata.size() )
					//strcpy( string_cache[i], &filedata.at(prev_pos) );
					string_cache[i] = filedata.substr(prev_pos);
				else
					//strcpy( string_cache[i], "");
					string_cache[i] = "";
			}
			else {
				//char * p_end = std::copy(&filedata.at(prev_pos),&filedata.at(pos), string_cache[i]);
				//*p_end = '\0';
				string_cache[i] = filedata.substr(prev_pos, pos - prev_pos);
			}

			pointer_array[i]->reset_data(string_cache[i].c_str());
			//std::cout << pointer_array[i]->get_class_name() << std::endl;
			//std::cout << pointer_array[i]->get_data().at(0)->c_str() << std::endl;
			//pointer_array[i]->print(std::cout);
			pAttrib = pointer_array[i]->clone();	//HERE CREATED NEW CLASS INSTANCES.
			//pAttrib->print(std::cout);
			temp_vec_attrib.push_back(pAttrib);
		}

		cRecord temprec(temp_vec_attrib);
		source.push_back(temprec);
		//temprec.print();
		++size;
		if ( size % base == 0 )
			std::cout << size << " records obtained." << std::endl;
	}
	std::cout << std::endl;
	std::cout << size << " records have been fetched from "<< txt_file << std::endl;

	//Reconfigure
	std::cout << "Reconfiguring ..." << std::endl;
	const cReconfigurator_AsianNames corrector_asiannames;
	const cReconfigurator_Latitude_Interactives corrector_lat_interactives;

	std::for_each(source.begin(), source.end(), corrector_asiannames);
	std::for_each(source.begin(), source.end(), corrector_lat_interactives);

	std::cout << "Reconfiguration done." << std::endl;

	//for ( vector <char *> ::iterator str_it = string_cache.begin(); str_it != string_cache.end(); ++ str_it )
		//delete [] (*str_it);

	for ( unsigned int i = 0; i < num_cols; ++i )
		delete pointer_array[i];
	delete [] pointer_array;

	return true;
}






