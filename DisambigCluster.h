/*
 * DisambigCluster.h
 *
 *  Created on: Jan 12, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGCLUSTER_H_
#define DISAMBIGCLUSTER_H_

#include <iostream>
#include <map>
#include <set>
#include "DisambigDefs.h"
#include <fstream>
#include <pthread.h>
#include "Threading.h"
#include "DisambigNewCluster.h"

using std::string;
using std::set;
using std::map;


//forward declaration
class cRecord;
class cRatios;

/*
 * cCluster_Info:
 * This class is a key class for the whole disambiguation. It enables loading the previous disambiguation results from
 * a source file, grouping clusters by blocking, configuring priori probabilities for each block, running the disambiguation
 * process, and exporting the results to an external file that can be used for the next run.
 *
 * Private:
 * 		const map <string, const cRecord*> * const uid2record_pointer: the binary tree map. Key = string of unique record id
 * 																		Value = the record pointer whose unique record id is the Key.
 * 		const bool is_matching: a boolean indicating the status of the "this" object. Now it is always set to true.
 * 								However, for future expandability, the variable is still kept.
 * 		unsigned int total_num: total_number of records. It is used to verify the completeness of "this" object.
 * 		map < string, cRecGroup > cluster_by_block: the binary tree map. Key = blocking identifier. Value = a list of clusters
 * 													whose delegates' extracted information blocking string = Key.
 * 		vector < map < string, unsigned int > > column_stat: a vector of map: key = extracted string part by a Blocking_Operation
 * 														   object, value = its occurrence. The vector stores the statistics for
 * 															all the separate parts by the blocking_operation object.
 * 		map < const string *, list <double>  > prior_data: a binary map used to keep track of prior values. Key = pointer to the blocking id.
 * 														  Value = a list of double numbers that records the history of priors.
 * 		map < const string *, bool > block_activity: a binary map used to check whether disambiguation within a certain block should occur.
 * 													Key = pointer to the block id. Value = whether or not to disambiguate.
 * 		vector < unsigned int > max_occurrence: data collected from column_stat that stores the maximum occurrence of each blocking string part.
 * 		vector < unsigned int > max_occurrence: data collected from column_stat that stores the minimum occurrence of each blocking string part.
 * 		string useless: a string that stores only the delimiters of Blocking_Operation objects. In other words, blocking id of such string has no information at all.
 * 		const bool frequency_adjust_mode: whether to use frequency to adjust the prior of a certain block.
 * 		const bool debug_mode: debug mode. In debug mode, only certain blocks will be disambiguated, and the progress is printed.
 * 		vector < double > thresholds: thresholds. The disambiguation is running in a progressive way, so the threshold are expected to be
 * 									like [ 0.99, 0.98, 0.95, ...] in a descending manner.
 *
 *		class cException_Cluster_Error: exception type used in cCluster_Info.
 *		void config_prior(): config the priori probabilities.
 *		unsigned int disambiguate_by_block ( cRecGroup & to_be_disambiged_group,  list <double> & prior_value,
											const cRatios & ratiosmap, const string * const bid, const double threshold ):
							To disambiguate a certain block with all necessary information.
 *		void retrieve_last_comparision_info ( const cBlocking_Operation & blocker, const char * const past_comparision_file):
 *					To retrieve the disambiguation results from last run, and start blocking by "blocker".
 *		void output_prior_value( const char * const prior_to_save ) const: save the priori values and their blocking ids in a file.
 *		cCluster_Info ( const cCluster_Info &): copy constructor. It is forbidden to call copy constructor.
 *		void print(std::ostream & os) const: export the comparison to an ostream. Called only internally.
 *		const string & get_useless_string() const: return the reference of the "information-less" string by the blocker.
 *		double get_prior_value( const string & block_identifier, const list <cCluster> & rg ): obtain a priori probability for a certain block.
 *		const map < const string *, list<double> > & get_prior_map() const: return the const reference of priori probability map.
 *		map < const string*, list <double> > & get_prior_map(): returns the reference of the priori probability map.
 *		const cRecGroup & get_comparision_map(const string* bid) const: return the const reference of cluster lists whose blocking label pointer is bid.
 *		cRecGroup & get_comparision_map(const string* bid): return the reference of cluster lists whose blocking label pointer is bid.
 *		unsigned int reset_block_activity( const char * filename ): read data from the file and set the corresponding block to be active.
 *																	Returns the number of active blocks. This is designed for debugging.
 * Public:
 *		static const char * const primary_delim: the primary delimiter in the text file that saves disambiguation results.
 *		static const char * const secondary_delim: the secondary delimiter in the text file that saves disambiguation results.
 *		void output_current_comparision_info(const char * const outputfile ) const: output current comparison results to a file.
 *
 *		void reset_blocking(const cBlocking_Operation & blocker, const char * const past_comparision_file):
 *				Read the data from the specified file, and use the blocker to create blocks. A necessary step for disambiguation.
 *		void preliminary_consolidation(const cBlocking_Operation & blocker, const list < const cRecord *> & all_rec_list):
 *				A preliminary consolidation step. Put all the records with identical blocking string together in same clusters.
 *				In practice, it is used to group records with exact firstname, middlename, lastname, assignee, city, country together.
 *		cCluster_Info(const map <string, const cRecord*> & input_uid2record, const bool input_is_matching , const bool aum , const bool debug ):
 *				Constructor of a class object. input_uid2record = map of unique record id string to its cRecord object pointer.
 *				input_is_matching = whether the object reads a match file. Always set to true unless new functionalities are added.
 *				aum = frequency adjust mode: on or off. debug = debug mode: on or off.
 *		void disambiguate(const cRatios & ratiosmap, const unsigned int num_threads, const char * const debug_block_file, const char * const prior_to_save):
 *				The function that starts the disambiguation. ratiosmap = a cRatios object.
 *				num_threads = number of threads. For maximum performance it is recommended to be number of CPUs in the machine.
 *				debug_block_file = the file from which specific blocking ids are read and set active. If it is an invalid file, it is ignored.
 *				prior_to_save = the file to which the priori probability history of each block will be saved. Perfect for analysis and debugging.
 *		~cCluster_Info(): destructor.
 *		const map < string, cRecGroup> & get_cluster_map () const: return the variable cluster_by_block.
 *		bool is_matching_cluster() const: return whether the variable "is_matching" of the object.
 *		bool is_consistent() const: performs a consistency check of the object and returns the result.
 *		const vector < double > & set_thresholds ( const vector < double > & input ): set the threshold stages for the disambiguation.
 *				Since the disambiguation is a progressive process, the input vector is supposed to be monotonically descending.
 *				For example: 0.99, 0.98, 0.95, 0.90.
 *
 *
 *	Example of Use:
 *
 *	const cBlocking_Operation * pblocker;
 *	const map < string, const cRecord * > *puid_dict;
 *	//... ...
 *	//Here creates a concrete object of cBlocking_Operation subclass, and assigns the address to pblocker.
 *	//Here creates a map of string of unique record id to const cRecord pointer, and assigns to puid_dict.
 *	//... ...
 *	bool is_matching = true;
 *	bool frequency_adjust_mode = true;
 *	bool debug_mode = false;
 *	const unsigned int number_of_threads = 24; // set number of threads to 24 if the computer has 24 cores.
 *	cCluster_Info CIobj ( *puid_dict, is_matching, frequency_adjust_mode, debug_mode );	//create a cCluster_Info object.
 *	vector < double > thresholds;
 *	thresholds.push_back(0.99);
 *	thresholds.push_back(0.95);
 *  CIobj.set_thresholds(thresholds);		//set thresholds to the above double vector
 *	const char * previous_round_file = "disambig_r3.txt";
 *	CIobj.reset_blocking( *pblocker, previous_round_file );	//read the data into the object from specified file
 *	const cRatio * pratio;
 *	//......
 *	//Here create a cRatio object, do enough preparations for the object, and then assign pratio to the object.
 *	//This should include training steps.
 *	//......
 *	const char * debug_file = "invalid_file.txt";		//this file can be invalid and should be empty if a whole disambiguation is expected.
 *	const char * prior_history = "prior_history_r4.txt";	//this file will be created if not exists.
 *	CIobj.disambiguate( *pratio, number_of_threads, debug_file, prior_history );	//this command starts the disambiguation.
 *	//It should take a several hours to run the disambiguation, so waiting...
 *	//......
 *	//......
 *	const char * target_file = "disambig_r4.txt";	//now ready to dump the disambiguation results into this file.
 *	CIobj.output_current_comparision_info( target_file );		//dumping results.
 *
 *	//Now the target file has the result of the latest disambiguation.
 *	//It can be used as a source file for another round of disambiguation, if necessary.
 *	//However, remember that a new blocking mechanism and a new ratio ( perhaps a new training set, too) should be
 *	//present and exported to a cRatio object that serves as a ratio database.
 *
 *
 *
 */

class cCluster_Info {
	friend class cWorker_For_Disambiguation;
public:
	typedef set<const cRecord *> recordset;
	typedef list < cCluster > cRecGroup;

	friend bool disambiguate_wrapper(const map<string, cCluster_Info::cRecGroup>::iterator & p,cCluster_Info & cluster,
								const cRatios & ratiosmap );

private:

	const map <string, const cRecord*> * const uid2record_pointer;
	const bool is_matching;
	unsigned int total_num;

	map < string, cRecGroup > cluster_by_block;
	vector < map < string, unsigned int > > column_stat;
	map < const string *, list <double>  > prior_data;
	map < const string *, bool > block_activity;
	vector < unsigned int > max_occurrence;
	vector < unsigned int > min_occurrence;

	string useless;
	const bool frequency_adjust_mode;
	const bool debug_mode;
	vector < double > thresholds;


	class cException_Cluster_Error: public cAbstract_Exception {
	public:
		cException_Cluster_Error(const char* errmsg): cAbstract_Exception(errmsg){};
	};

	void config_prior();

	unsigned int disambiguate_by_block ( cRecGroup & to_be_disambiged_group,  list <double> & prior_value,
											const cRatios & ratiosmap, const string * const bid, const double threshold ) ;
	void retrieve_last_comparision_info ( const cBlocking_Operation & blocker, const char * const past_comparision_file);
	void output_prior_value( const char * const prior_to_save ) const;
	cCluster_Info ( const cCluster_Info &);
	void print(std::ostream & os) const;
	const string & get_useless_string() const {return useless;}
	double get_prior_value( const string & block_identifier, const list <cCluster> & rg );
	const map < const string *, list<double> > & get_prior_map() const {return prior_data;}
	map < const string*, list <double> > & get_prior_map() {return prior_data;};
	const cRecGroup & get_comparision_map(const string* bid) const;
	cRecGroup & get_comparision_map(const string* bid);
	unsigned int reset_block_activity( const char * filename );

public:

	static const char * const primary_delim;
	static const char * const secondary_delim;

	void output_current_comparision_info(const char * const outputfile ) const;
	void reset_blocking(const cBlocking_Operation & blocker, const char * const past_comparision_file);
	void preliminary_consolidation(const cBlocking_Operation & blocker, const list < const cRecord *> & all_rec_list);
	cCluster_Info(const map <string, const cRecord*> & input_uid2record, const bool input_is_matching , const bool aum , const bool debug );
	void disambiguate(const cRatios & ratiosmap, const unsigned int num_threads, const char * const debug_block_file, const char * const prior_to_save);
	~cCluster_Info() {}
	const map < string, cRecGroup> & get_cluster_map () const {return cluster_by_block;}
	bool is_matching_cluster() const {return is_matching;};
	bool is_consistent() const;
	const vector < double > & set_thresholds ( const vector < double > & input );
};


/*
 * cWorker_For_Disambiguation:
 * This class is a threading subclass to achieve multithreading in Linux systems.
 * It is used in cCluster_Info::disambiguate function.
 * It is unnecessary to understanding the detail. The only thing necessary to know is the constructor.
 *
 * Private:
 * 		map < string, cCluster_Info::cRecGroup >::iterator * ppdisambiged: the pointer to an iterator that is a cursor of progress of disambiguation.
 *		const cRatios * pratios: the pointer to a cRatio object.
 *		cCluster_Info & cluster_ref: the reference of a cCluster_Info object that is actually the source.
 *		static pthread_mutex_t iter_lock: a mutex to synchronize the cursor.
 *		static unsigned int count: a static member to count the number of disambiguated blocks.
 *		void run(): the overriding function of base class, implementing details of disambiguation in each thread.
 *
 * Public:
 * 		explicit cWorker_For_Disambiguation( map < string, cCluster_Info::cRecGroup >::iterator & input_pdisambiged,
											const cRatios & ratiosmap, cCluster_Info & inputcluster): constructor.
 *		~cWorker_For_Disambiguation(): destructor.
 *		static void zero_count(): clear the variable "count" to zero.
 *		static unsigned int get_count(): return the variable "count".
 *
 */

class cWorker_For_Disambiguation : public Thread {
private:
	map < string, cCluster_Info::cRecGroup >::iterator * ppdisambiged;
	const cRatios * pratios;
	cCluster_Info & cluster_ref;

	static pthread_mutex_t iter_lock;
	static unsigned int count;
	void run();
public:
	explicit cWorker_For_Disambiguation( map < string, cCluster_Info::cRecGroup >::iterator & input_pdisambiged,
			const cRatios & ratiosmap,
			cCluster_Info & inputcluster
	) : ppdisambiged(&input_pdisambiged), pratios(&ratiosmap), cluster_ref(inputcluster) {}

	~cWorker_For_Disambiguation() {}
	static void zero_count() { count = 0; }
	static unsigned int get_count() { return count;}
};


#endif /* DISAMBIGCLUSTER_H_ */
