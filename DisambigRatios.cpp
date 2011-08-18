/*
 *  DisambigRatios.cpp
 *  mydisambiguation

 *
 */

#include "DisambigRatios.h"
#include "DisambigEngine.h"
#include <cstring>


const char * cRatios::primary_delim = "#";
const char * cRatios::secondary_delim = ",";
const unsigned int cRatioComponent::laplace_base = 5;


vector < unsigned int > get_max_similarity(const vector < string > & attrib_names)  {
	vector < unsigned int > sp;
	for ( vector < string > :: const_iterator p = attrib_names.begin(); p != attrib_names.end(); ++p ) {
		const cAttribute * pAttrib = cRecord::get_sample_record().get_attrib_pointer_by_index(cRecord::get_index_by_name(*p));
		const unsigned int max_entry = pAttrib->get_attrib_max_value();
		sp.push_back(max_entry);
	}
	return sp;
}


void cRatioComponent::sp_stats (const list<std::pair<string, string> > & trainpairs, 
						map < vector < unsigned int > , unsigned int, cSimilarity_Compare > & sp_counts ) const{
	const vector < unsigned int > & component_indice_in_record = get_component_positions_in_record();
	//const list <cRecord > & source = *psource;
	//cSort_by_attrib unique_comparator(unique_identifier);
	//const unsigned int unique_index = cRecord::get_index_by_name(unique_identifier);
	
	/*
	map <string, const cRecord *> dict;
	map<string, const cRecord *>::iterator pm;
	for ( list<cRecord>::const_iterator p = source.begin(); p != source.end(); ++p ) {
		const cAttribute *pAttrib = p->get_attrib_pointer_by_index(unique_index); 
		if ( pAttrib->get_data().size() != 1 ) 
			throw cException_Vector_Data(pAttrib->get_class_name().c_str());
		const string & info = pAttrib->get_data().at(0);
		pm = dict.find( info);
		if ( pm != dict.end() )
			throw cException_Invalid_Attribute_For_Sort( pAttrib->get_class_name().c_str());
		dict.insert(std::pair<string, const cRecord*>(info, &(*p) ));
	}
	 */
	const map <string, const cRecord *> & dict = *puid_tree;
	map<string, const cRecord *>::const_iterator pm;
	map < vector < unsigned int >, unsigned int, cSimilarity_Compare >::iterator psp;
	for ( list< std::pair<string, string> >::const_iterator p = trainpairs.begin(); p != trainpairs.end(); ++p ) {
		pm = dict.find(p->first);
		if ( pm == dict.end() )
			throw cException_Attribute_Not_In_Tree( ( string("\"") + p->first + string ("\"") ).c_str() );
		const cRecord *plhs = pm->second;
		pm = dict.find(p->second);
		if ( pm == dict.end() )
			throw cException_Attribute_Not_In_Tree( ( string("\"") + p->second + string ("\"") ).c_str() );
		const cRecord *prhs = pm->second;
		
		vector < unsigned int > similarity_profile = plhs->record_compare_by_attrib_indice(*prhs, component_indice_in_record);
		//debug only
		//std::cout << "Size of Similarity Profile = "<< similarity_profile.size() << ". Similarity Profile = ";
		//for ( vector <unsigned int >::const_iterator tt = similarity_profile.begin(); tt != similarity_profile.end(); ++tt )
		//	std::cout << *tt << ":";
		//std::cout << std::endl;
		//end of debug
		psp = sp_counts.find(similarity_profile);
		if ( psp == sp_counts.end() )
			sp_counts.insert( std::pair < vector <unsigned int>, unsigned int >(similarity_profile, 1));
		else {
			++ (psp->second);
		}
	}
}

void cRatioComponent::read_train_pairs(list<std::pair<string, string> > & trainpairs, const char * txt_file) const {
	static const char * delim = ",";
	static const unsigned int delim_size = strlen(delim);
	
	std::ifstream::sync_with_stdio(false);
	std::ifstream infile(txt_file);
	if (infile.good()) {
		string filedata;
		while ( getline(infile, filedata)) {
			register size_t pos = 0, prev_pos = 0;
			pos = filedata.find(delim, prev_pos);
			string firststring = filedata.substr( prev_pos, pos - prev_pos);
			prev_pos = pos + delim_size;
			pos = filedata.find(delim, prev_pos);
			string secondstring = filedata.substr(prev_pos, pos);
			trainpairs.push_back( std::pair<string, string>(firststring, secondstring) );
		}
		std::cout << txt_file << " has been loaded as the " << attrib_group << " part of the training sets."<< std::endl;
	}
	else {
		throw cException_File_Not_Found(txt_file);
	}
}

void cRatioComponent::stats_output( const char * filename) const {

	std::ofstream of ( filename );
	const string nmc = "Non-Match Counts";
	const string mc = "Match Counts";
	const string splabel = "Similarity Profiles";
	const string delim = " | ";

	of << splabel  << "(";
	for ( vector < unsigned int >:: const_iterator tt = this->positions_in_record.begin(); tt != this->positions_in_record.end(); ++tt )
		of << cRecord::get_column_names().at(*tt) << ",";
	of << ")";

	of << delim << mc << delim << nmc << '\n';
	map < vector < unsigned int >, unsigned int  >::const_iterator pm;
	map < vector < unsigned int >, double>::const_iterator p;
	for ( p = this->ratio_map.begin(); p != this->ratio_map.end(); ++p ) {
		for ( vector <unsigned int >::const_iterator q = p->first.begin(); q != p->first.end(); ++q )
			of << *q << ",";
		of << delim;
		pm = this->m_counts.find(p->first);
		of << pm->second << delim;
		pm = this->x_counts.find(p->first);
		of << pm->second << '\n';
	}

	for ( pm = this->m_counts.begin(); pm != this->m_counts.end(); ++pm ) {
		p = this->ratio_map.find(pm->first);
		if ( p != this->ratio_map.end())
			continue;
		for ( vector <unsigned int >::const_iterator q = pm->first.begin(); q != pm->first.end(); ++q )
			of << *q << ",";
		of << delim;
		of << pm->second << delim;
		of << 0 << '\n';
	}

	for ( pm = this->x_counts.begin(); pm != this->x_counts.end(); ++pm ) {
		p = this->ratio_map.find(pm->first);
		if ( p != this->ratio_map.end())
			continue;
		for ( vector <unsigned int >::const_iterator q = pm->first.begin(); q != pm->first.end(); ++q )
			of << *q << ",";
		of << delim;
		of << 0 << delim;
		of << pm->second << '\n';
	}
	std::cout << "Ratio component info saved in " << filename << std::endl;
}


void cRatioComponent::prepare(const char* x_file, const char * m_file) {
	list<std::pair<string, string> > x_list, m_list;
	x_counts.clear();
	m_counts.clear();
	ratio_map.clear();

	get_similarity_info();

	read_train_pairs(x_list, x_file);
	read_train_pairs(m_list, m_file);
	sp_stats(x_list, x_counts);
	sp_stats(m_list, m_counts);
	

	std::cout << "Before LAPLACE CORRECTION: " << std::endl;
	std::cout << "Size of non-match pair list = " << x_list.size() << std::endl;
	std::cout << "Size of match pair list = " << m_list.size() << std::endl;
	
	std::cout << "Non-match unique profile number = " << x_counts.size() << std::endl;
	std::cout << "Match unique profile number = " << m_counts.size() << std::endl;
	
	// laplace correction
	map < vector < unsigned int >, unsigned int, cSimilarity_Compare >::const_iterator p, q;

	const unsigned int count_to_consider = 100;
	set < vector < unsigned int >, cSimilarity_Compare > all_possible;
	for ( p = x_counts.begin(); p != x_counts.end(); ++p ) {
		if ( m_counts.find(p->first) == m_counts.end() && p->second < count_to_consider )
			continue;
		else
			all_possible.insert(p->first);
	}
	for ( p = m_counts.begin(); p != m_counts.end(); ++p ) {
		if ( x_counts.find(p->first) == x_counts.end() && p->second < count_to_consider )
			continue;
		else
			all_possible.insert(p->first);
	}

	for ( set< vector < unsigned int >, cSimilarity_Compare >::const_iterator ps = all_possible.begin(); ps != all_possible.end(); ++ps ) {
		map < vector < unsigned int >, unsigned int, cSimilarity_Compare >::iterator p = x_counts.find(*ps);
		if ( p == x_counts.end() )
			x_counts.insert(std::pair< vector< unsigned int>, unsigned int >(*ps, laplace_base));
		else
			p->second += laplace_base;

		p = m_counts.find(*ps);
		if ( p == m_counts.end() )
			m_counts.insert(std::pair< vector< unsigned int>, unsigned int >(*ps, laplace_base));
		else
			p->second += laplace_base;
	}
	std::cout << "AFTER LAPLACE CORRECTION:" << std::endl;
	std::cout << "Non-match unique profile number = " << x_counts.size() << std::endl;
	std::cout << "Match unique profile number = " << m_counts.size() << std::endl;
	//ratios = count of match / count of non-match;
	
	
	unsigned int num_xcount_without_mcount = 0;
	unsigned int num_mcount_without_xcount = 0;
	for ( p = x_counts.begin(); p != x_counts.end(); ++p ) {
		q = m_counts.find( p->first );
		if ( q == m_counts.end() ) 
			continue;
		else {
			ratio_map.insert(std::pair< vector < unsigned int>, double >(p->first, 1.0 * q->second / p->second) );
		}
	}

	for ( map < vector < unsigned int >, unsigned int, cSimilarity_Compare >::iterator pp = x_counts.begin(); pp != x_counts.end();  ) {
		if ( ratio_map.find( pp->first ) == ratio_map.end() ) {
			x_counts.erase ( pp++ );
			++num_xcount_without_mcount;
		}
		else
			++pp;
	}
	for ( map < vector < unsigned int >, unsigned int, cSimilarity_Compare >::iterator qq = m_counts.begin(); qq != m_counts.end();  ) {
		if ( ratio_map.find( qq->first ) == ratio_map.end() ) {
			m_counts.erase ( qq++ );
			++num_mcount_without_xcount;
		}
		else
			++qq;
	}

	std::cout << num_xcount_without_mcount << " non-match similarity profiles and "
			<< num_mcount_without_xcount << " match similarity profiles are discarded." << std::endl;

	if ( num_xcount_without_mcount > 5 || num_mcount_without_xcount > 5 ) {
		std::cout << "WARNING: THIS STEP IS SKIPPED FOR DEBUG BUT SHALL BE ENABLED IN THE REAL PROGRAM, UNLESS SMOOTHING AND INTER-EXTRA-POLATIONS ARE AVAILABLE."<<std::endl;
		std::cout << "Discovered " << num_xcount_without_mcount << " non-match similarity profiles that are not available in matched ones. "
		<< std::endl << "And " << num_mcount_without_xcount << " match similarity profiles that are not available in non-match ones."
		<< std::endl;
		//throw cException_Partial_SP_Missing(attrib_group.c_str());
	}
	

	smooth();
	similarity_map.clear();
	is_ready = true;
}


cRatioComponent:: cRatioComponent( const map < string, const cRecord * > & uid_tree, const string & groupname)
			: attrib_group(groupname), puid_tree(&uid_tree), is_ready(false) {
};

void cRatioComponent::get_similarity_info() {
	positions_in_ratios.clear();
	positions_in_record.clear();
	attrib_names.clear();
	//const cRecord & sample_record = psource->front();
	const cRecord & sample_record = cRecord::get_sample_record();
	static const string useless_group_label = "None";
	unsigned int ratios_pos = 0, record_pos = 0;
	for ( vector<const cAttribute*>::const_iterator p = sample_record.vector_pdata.begin(); p != sample_record.vector_pdata.end(); ++p) {
		const string & info = (*p)->get_attrib_group();
		bool comparator_activated = (*p)->is_comparator_activated();
		//std::cout << (*p)->get_class_name() << std::endl;
		if ( info == attrib_group && comparator_activated) {
			positions_in_ratios.push_back(ratios_pos);
			positions_in_record.push_back(record_pos);
			attrib_names.push_back(cRecord::get_column_names().at(record_pos));
		}
		if ( info != useless_group_label && comparator_activated )
			++ratios_pos;
		++ record_pos;
	}
}

/*
void cRatios::Get_Coefficients() {
	// y = a1x1 + a2x2 + ... + anxn + a0(-1)
	double R_square = 0;
	vector < vector < double> > mat;
	vector < double > b;
 	for ( map < vector < unsigned int > , double, cSimilarity_Compare >::const_iterator p = final_ratios.begin(); p != final_ratios.end(); ++p ) {
		vector<double > row;
		for ( vector < unsigned int >:: const_iterator q = p->first.begin(); q != p->first.end();++q)
			row.push_back(*q);
		//ATTENTION HERE
		row.push_back(1);
		mat.push_back(row);

	}

 	double max_rsq = -10;
 	unsigned int root_order;
 	vector < double> temp_coeff;
 	for (  root_order = 1; root_order < 10; ++ root_order) {
 		b.clear();
 		for ( map < vector < unsigned int > , double, cSimilarity_Compare >::const_iterator p = final_ratios.begin(); p != final_ratios.end(); ++p )
 			b.push_back(pow(p->second, 1.0/root_order));
 		temp_coeff = NNLS_fit(mat,b, R_square);
 		if ( R_square > max_rsq) {
 			max_rsq = R_square;
 			final_root_order = root_order;
 			coeffs = temp_coeff;
 		}
 	}
 	if ( R_square < 0.5 ) {
 		std::cout << "WARINING: R-SQUARE OF COEFFICIENTS IS SMALL. THE LINEAR RELATION IS PROBABLY PROBLEMATIC." << std::endl;
 		final_root_order = 0;
 		std::cout << "USING UNSMOOTHED RAW COUNTS INSTEAD." << std::endl;
 	}
}

*/


cRatios:: cRatios(const vector < const cRatioComponent *> & component_pointer_vector, const char * filename, const cRecord & rec) {
	std::cout << "Creating the final version ratios file ..." << std::endl;
	unsigned int ratio_size = 0;
	for ( vector< const cRatioComponent *>::const_iterator p = component_pointer_vector.begin(); p != component_pointer_vector.end(); ++p ) {
		std::cout << " Size of Ratio Component = " << (*p)->get_ratios_map().size() << std::endl;
		ratio_size += (*p)->get_component_positions_in_ratios().size();
	}
	
	attrib_names.resize(ratio_size, "Invalid Attribute");
	static const unsigned int impossible_value = 10000;
	vector<unsigned int> null_vect (ratio_size, impossible_value);
	final_ratios.insert( std::pair<  vector <unsigned int> , double > (null_vect, 1) );
	x_counts.insert(std::pair<  vector <unsigned int> , unsigned int > (null_vect, 0));
	m_counts.insert(std::pair<  vector <unsigned int> , unsigned int > (null_vect, 0));

	
	for ( vector< const cRatioComponent *>::const_iterator p = component_pointer_vector.begin(); p != component_pointer_vector.end(); ++p ) {
		More_Components(**p);
	}
	
	// now checking the final ratios
	const vector < unsigned int > & firstline = final_ratios.begin()->first;
	for ( vector< unsigned int >::const_iterator k = firstline.begin(); k < firstline.end(); ++k ) {
		if ( *k == impossible_value )
			throw cRatioComponent::cException_Ratios_Not_Ready( "Final Ratios is not ready yet. ");
	}
	

	// smoothing here
	smooth();
	//const vector < unsigned int > max_similarity = get_max_similarity( this->attrib_names);
	//const vector < unsigned int > min_similarity ( max_similarity.size(), 0);
	//inter_extra_polation(max_similarity, min_similarity);

	write_ratios_file(filename);
	x_counts.clear();
	m_counts.clear();
	similarity_map.clear();
}

cRatios:: cRatios(const char * filename) {
	read_ratios_file(filename);
}


void cRatios::More_Components( const cRatioComponent & additional_component) {
	map < vector <unsigned int>, double, cSimilarity_Compare > temp_ratios;
	map < vector < unsigned int > , unsigned int, cSimilarity_Compare > temp_x_counts, temp_m_counts;
	const vector < unsigned int > & temp_pos_in_rec = additional_component. get_component_positions_in_record();
	const vector < unsigned int > & positions_in_ratios = additional_component.get_component_positions_in_ratios();
	for ( unsigned int k = 0; k < positions_in_ratios.size(); ++k ) {
		attrib_names.at( positions_in_ratios.at(k) ) = cRecord::get_column_names().at(temp_pos_in_rec.at(k) );
	}
	
	for (map < vector <unsigned int>, double, cSimilarity_Compare >::iterator p = final_ratios.begin(); p != final_ratios.end(); ++p ) {
		vector < unsigned int > key = p->first;
		for ( map < vector <unsigned int>, double, cSimilarity_Compare >::const_iterator vv = additional_component.get_ratios_map().begin();
			 vv != additional_component.get_ratios_map().end(); ++vv) {
			for ( unsigned int j = 0; j < vv->first.size(); ++j ) {
				key.at( positions_in_ratios.at(j) ) = vv->first.at(j);
			}
			temp_ratios.insert( std::pair < vector < unsigned int >, double >(key, p->second * vv->second));
			temp_x_counts.insert(std::pair < vector < unsigned int >, unsigned int >(key, this->x_counts.find(p->first)->second + additional_component.get_x_counts().find(vv->first)->second ) );
			temp_m_counts.insert(std::pair < vector < unsigned int >, unsigned int >(key, this->m_counts.find(p->first)->second + additional_component.get_m_counts().find(vv->first)->second ) );
		}
	}
	final_ratios = temp_ratios;
	x_counts = temp_x_counts;
	m_counts = temp_m_counts;
}


void cRatios::write_ratios_file( const char * filename) const {
	std::ofstream::sync_with_stdio(false);
	std::cout << "Number of of ratios vectors = " << final_ratios.size() << std::endl;
	std::cout << "Saving the ratios in the file " << filename << std::endl;
	
	std::ofstream outfile(filename);
	for (vector<string>::const_iterator p = attrib_names.begin(); p != attrib_names.end(); ++p ) {
		outfile << *p << secondary_delim;
	}
	outfile << primary_delim << "VALUE" << '\n';
	
	for (map < vector <unsigned int>, double, cSimilarity_Compare >::const_iterator q = final_ratios.begin(); 
		 q != final_ratios.end(); ++q ) {
		for ( vector < unsigned int>::const_iterator pint = q->first.begin(); pint != q->first.end(); ++pint) 
			outfile << *pint << secondary_delim;
		outfile << primary_delim << q->second << '\n';
	}
	std::cout << "Ratios file saved." << std::endl;
}

void cRatios::read_ratios_file(const char * filename) {
	std::ifstream::sync_with_stdio(false);
	std::ifstream infile ( filename );
	const unsigned int primary_delim_size = strlen(primary_delim);
	const unsigned int secondary_delim_size = strlen(secondary_delim);
	if ( ! infile.good() )
		throw cException_File_Not_Found(filename);
	
	string filedata;
	register size_t pos, prev_pos;
	getline(infile, filedata);
	pos = prev_pos = 0;
	while ( ( pos = filedata.find(secondary_delim, prev_pos) ) != string::npos ) {
		attrib_names.push_back(filedata.substr(prev_pos, pos - prev_pos));
		prev_pos = pos + secondary_delim_size;
	}
	
	vector < unsigned int > key;
	while ( getline(infile, filedata)) {
		key.clear();
		pos = prev_pos = 0;
		while ( ( pos = filedata.find(secondary_delim, prev_pos) ) != string::npos ) {
			key.push_back(atoi(filedata.substr(prev_pos, pos - prev_pos).c_str()));
			prev_pos = pos + secondary_delim_size;
		}

		
		pos = filedata.find(primary_delim, 0);
		pos += primary_delim_size;
		
		const double value = atof(filedata.substr(pos).c_str());
		
		final_ratios.insert(std::pair<vector<unsigned int>, double >(key, value));
	}
	std::cout << filename << " has been loaded as the final ratios file"<< std::endl;
	std::cout << "Resetting similarity profiles ... ..." << std::endl;
	cRecord::activate_comparators_by_name(attrib_names);
	std::cout << "-----Similarity Profiles reset.-------" << std::endl;
}
