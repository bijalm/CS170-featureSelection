#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

void forward(vector< vector<float>> data);
void backward(vector< vector<float>> data);
vector< vector<float> > readFile(string fileName);
double leave_one_out_cross_validation(vector< vector<float>>, vector<float>, int, int);
bool hasFeature(vector<float>,int);
vector<float> removeFeature(vector<float>, int);

int main() {
	string fileName;
	int choice = 0;
	vector< vector<float> > data;

	time_t start, stop;
	//Ver_2_CS170_Fall_2021_Small_data__15.txt
	//Ver_2_CS170_Fall_2021_LARGE_data__8.txt

	cout << "Welcome to Bijal Mugatwala's Feature Selection Algorithm." << endl;
	cout << "Type in the name of the file to test : ";
	cin >> fileName;
	cout << endl;

	data = readFile(fileName);

	cout << "Type in the number of the algorithm you want to run." << endl;
	cout << "\t1) Forward Selection\n\t2) Backward Elimination" << endl;
	cin >> choice;

	cout << "This dataset has " << data[0].size()-1 << " features, and " << data.size() << " instances." << endl;
	time(&start);
	if (choice == 1){
		forward(data);
	}
	else if (choice == 2){
		backward(data);
	}
	else {
		cout << "Incorrect choice" << endl;
		return -1;
	}
	time(&stop);
	double algTime = double(stop - start);
	cout << "Running Time: " << algTime << "seconds." << endl;

	return 0;
}

vector< vector<float> > readFile(string fileName) {
	vector< vector<float> > d;
	string line;

	ifstream ifs;
	ifs.open(fileName);

	if (!ifs.is_open()) {
		cout << "Couldn't open " << fileName << endl;
		exit(0);
	}

	while (getline(ifs, line)) {
		istringstream iss(line);
		string f;
		vector<float> inner;
		while (iss >> f){
			inner.push_back(stof(f));
		}
		d.push_back(inner);
	}
	ifs.close();
	return d;
}

void forward(vector< vector<float>> data) {
	cout << "Beginning search." << endl;
	vector<float> current_set_of_features;
	vector<float> best_set_of_features;
	double accuracy = 0;
	double best_so_far_accuracy = 0;
	double best_accuracy = 0;
	int feature_to_add_at_this_level = 0;

	for (int i = 1; i < data[0].size(); i++) {
		cout << "On the " << i << "th level of the search tree" << endl;

		feature_to_add_at_this_level = 0;
		best_so_far_accuracy = 0;

		for (int k = 1; k < data[i].size(); k++) {
			//add if the current_set_of_features doesnt have the kth feature
			//add only if the feature increases accuracy
			if (hasFeature(current_set_of_features, k) == false) {
				//cout << "--Considering add the " << k << " feature" << endl;
				accuracy = leave_one_out_cross_validation(data, current_set_of_features, k, 1);

				if (accuracy > best_so_far_accuracy) {
					best_so_far_accuracy = accuracy;
					feature_to_add_at_this_level = k;
				}
			}
			cout << "\tUsing feature(s) {";
			for (int j = 0; j < current_set_of_features.size(); j++) {
				cout << current_set_of_features.at(j) << ", ";
			} 
			cout << "} accuracy is " << accuracy * 100 << "%" << endl;
		}
		current_set_of_features.push_back(feature_to_add_at_this_level);
		if (best_so_far_accuracy > best_accuracy) {
			best_accuracy = best_so_far_accuracy;
			best_set_of_features = current_set_of_features;
		}
		else {
			cout << "Warning: Accuracy has decreased" << endl;
		}
		cout << "On level " << i << " I added feature " << feature_to_add_at_this_level << " to current set" << endl;

	}
	cout << "Finished Search!! The best feature subset is {";
	for (int i = 0; i < best_set_of_features.size(); i++) {
		cout << best_set_of_features.at(i) << ", ";
	}
	cout << "}, which has an accuracy of " << (best_accuracy * 100) << "%" << endl;
}


void backward(vector<vector<float>> data) {
	vector<float> current_set_of_features;
	vector<float> best_set_of_features;
	vector<float> features_removed;
	double accuracy = 0;
	double best_so_far_accuracy = 0;
	double best_accuracy = 0;
	int feature_to_remove = 0;


	for (int i = 1; i < data[0].size(); i++) {
		current_set_of_features.push_back(i);
		best_set_of_features.push_back(i);
	}

	for (int i = 1; i < data[0].size(); i++) {
		cout << "On the " << i << "th level of the search tree" << endl;

		feature_to_remove = 0;
		best_so_far_accuracy = 0;

		for (int k = 1; k < data[0].size(); k++) {
		    //check if the set has the feature, if so remove if accuracy is higher without it
			if (hasFeature(current_set_of_features, k) == true) { 
				//cout << "--Considering add the " << k << " feature" << endl;
				accuracy = leave_one_out_cross_validation(data, current_set_of_features, k, 2);
				current_set_of_features = removeFeature(current_set_of_features, k);

				if (accuracy >= best_so_far_accuracy) {
					best_so_far_accuracy = accuracy;
					feature_to_remove = k;
				}

				cout << "\tUsing feature(s) {";
				for (int j = 0; j < current_set_of_features.size(); j++) {
					cout << current_set_of_features.at(j) << ", ";
				}
				cout << "} accuracy is " << accuracy * 100 << "%" << endl;
			}
		}
		current_set_of_features = removeFeature(current_set_of_features, feature_to_remove);
		if (best_so_far_accuracy > best_accuracy) {
			best_accuracy = best_so_far_accuracy;
			best_set_of_features = current_set_of_features;
		}
	}
	
	cout << "Finished Search!! The best feature subset is {";
	for (int i = 0; i < best_set_of_features.size(); i++) {
		cout << best_set_of_features.at(i) << ", ";
	}
	cout << "}, which has an accuracy of " << (best_accuracy * 100) << "%" << endl;

}


bool hasFeature(vector<float> c, int k) {
	for (int i = 0; i < c.size(); i++) {
		if (c.at(i) == k) {
			return true;
		}
	}
	return false;
}

vector<float> removeFeature(vector<float> c, int f) {
	vector<float> removedFeature_set;
	for (int i = 0; i < c.size(); i++) {
		if (i != f) {
			removedFeature_set.push_back(c[i]);
		}
	}
	return removedFeature_set;
}

double leave_one_out_cross_validation(vector< vector<float>> d, vector<float> c, int f, int alg) {
	vector< vector<float> > tempD;
	double numCorrect = 0;
	double label = 0;
	double nnDistance;
	double nnLocation;
	double nnLabel;
	double distance;
	double currLabel;
	double sum;
	double diff;

	if (alg == 1) { //forward
		for (int i = 0; i < d.size(); i++) {
			vector<float> features;

			for (int j = 0; j < d[i].size(); j++) {
				if (hasFeature(c, j) || (j == f)) { //remove unnecessary features
					features.push_back(d[i][j]);
				}
				else {
					features.push_back(0);
				}
			}
			tempD.push_back(features);
		}
	}
	else { //backward
		for (int i = 0; i < d.size(); i++) {
			vector<float> features;
			
			for (int j = 1; j < d[i].size(); j++) {
				if (hasFeature(c, j) == false) {
					if (j = f) { //remove unnecessary features
						features.push_back(0);
					}
					else {
						features.push_back(d[i][j]);
					}
				}
				else {
					features.push_back(0);
				}
			}
			tempD.push_back(features);
		}

	}

	
	for (int i = 0; i < d.size(); i++) {
		label = d[i][0];

		nnDistance = DBL_MAX;
		nnLocation = DBL_MAX;
		nnLabel = 0;

		for (int j = 0; j < d.size(); j++) { //euclidean distance formula to find accuracy of classification
			if (j != i) {
				distance = 0;
				currLabel = d[j][0];

				sum = 0;
				for (int k = 0; k < tempD[0].size(); k++) {

					diff = tempD[i].at(k) - tempD[j].at(k);
					sum += pow(diff, 2);
				}

				distance = sqrt(sum);

				if (distance < nnDistance) {
					nnDistance = distance;
					nnLocation = j;
					nnLabel = d[nnLocation][0];
				}
			}
		}
		if (label == nnLabel) {
			numCorrect += 1;
		}
	}
	double accuracy = numCorrect / (d.size());
	return accuracy;
}
