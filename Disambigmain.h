/*
 * Disambigmain.h
 *
 *  Created on: Jan 10, 2011
 *      Author: ysun
 */

#ifndef DISAMBIGMAIN_H_
#define DISAMBIGMAIN_H_




int main_to_go();
int main_to_dump(const char * source_txt, const char * target_sqlite3);
int main_to_train();
int fullrun();
int fullrun_iterative();
int fullrun_iterative_v2();

bool make_changable_training_sets_by_names(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
bool make_stable_training_sets_by_personal ( const list <cRecord> & all_records, const unsigned int limit, const vector <string> & training_filenames);
bool make_changable_training_sets_by_assignee(const list <const cRecord*> & record_pointers, const vector<string >& blocking_column_names,
						const vector < const cString_Manipulator *> & pstring_oper, const unsigned int limit, const vector <string> & training_filenames);
#endif /* DISAMBIGMAIN_H_ */
