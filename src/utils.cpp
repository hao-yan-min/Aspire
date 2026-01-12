#include "utils.h"


using namespace std;
// RealDatabase::RealDatabase(uint64_t number_of_keywords,uint64_t keywords_bit_length,uint64_t value_bit_length,bool debug){
//     this->number_of_keywords = number_of_keywords;
//     this->keyword_bit_length = keywords_bit_length;
//     this->value_bit_length = value_bit_length;

//     this->keyword_hex_length = ceil(float(keywords_bit_length)/4);
//     this->value_hex_length = ceil(float(value_bit_length)/4);
    
//     srand((unsigned)time(NULL)); //set seed for random generator
    
//     set<string> keywords;
//     if (debug==true){
//         cout<< "Generate Keywords Set." << endl;
//     }
//     // generate random keywords
//     for(int i=0;i<this->number_of_keywords;i++){
        
//         string temp_string = generate_random_hex_string(this->keyword_hex_length);
//         string zero_string(this->keyword_hex_length,'0');
//         while (keywords.count(temp_string)>0 or temp_string == zero_string){
//             temp_string = generate_random_hex_string(this->keyword_hex_length);
//         }
//         keywords.insert(temp_string);
//     }
    
//     if (debug==true){
//         cout<< "Generate Keyword-Value Paires." << endl;
//     }
//     // generate random keyword-value pairs
//     for(auto it=keywords.begin();it!=keywords.end();++it){
//         this->keywords.push_back(*it);
//         string value = generate_random_hex_string(this->value_hex_length);
//         this->values.push_back(value);
//         this->keywords_values_map[*it] = value;
//     }
// }


Database::Database(uint64_t number_of_keywords,uint64_t keywords_bit_length,uint64_t value_bit_length){
    this->number_of_keywords = number_of_keywords;
    this->keyword_bit_length = keywords_bit_length;
    this->value_bit_length = value_bit_length;


    this->keyword_hex_length = ceil(float(keywords_bit_length)/4);
    this->value_hex_length = ceil(float(value_bit_length)/4);
    
    srand((unsigned)time(NULL)); //set seed for random generator
    

    set<string> keywords;
    cout<< "Generate Keywords Set." << endl;
    for(int i=0;i<this->number_of_keywords;i++){
        
        string temp_string = generate_random_hex_string(this->keyword_hex_length);
        string zero_string(this->keyword_hex_length,'0');
        while (keywords.count(temp_string)>0 or temp_string == zero_string){
            temp_string = generate_random_hex_string(this->keyword_hex_length);
        }




        keywords.insert(temp_string);
    }

    cout << "\nGenerate Keyword-Value Paires." << endl;
    for(auto it=keywords.begin();it!=keywords.end();++it){
        this->keywords.push_back(*it);
        string value = generate_random_hex_string(this->value_hex_length);
        this->values.push_back(value);
        this->keywords_values_map[*it] = value;
        //this->values.push_back(*it);
    }
    cout << "Generate Keyword-Value Paires finished."<<endl;
}

string Database::get_value_from_index(uint64_t index){//get value from index
    return this->values[index];
}
string Database::get_value_from_keyword(string keyword){//get value from keyword
    string keyword_upper = toUpper(keyword);
    if(this->keywords_values_map.find(keyword_upper)!= this->keywords_values_map.end()){
        return this->keywords_values_map[keyword_upper];
    }
    // for(uint64_t i=0;i<this->number_of_keywords;i++){
    //     if(toLower(this->keywords[i])==toLower(keyword)){
    //         return values[i];
    //     }
    // }
    return "";
}

string Database::get_random_keyword(){//get a random keyword that is in the database 
    uint64_t index = rand()%(this->number_of_keywords);
    return this->keywords[index];
}

vector<string> Database::get_multi_random_keyword(uint64_t number){//get random keywords that are in the database
    vector<string> keyword_vector;
    for (uint64_t i=0;i<number;i++){
        uint64_t index = rand()%(this->number_of_keywords);
        string keyword = this->keywords[index];
        while (find(keyword_vector.begin(),keyword_vector.end(),keyword)!=keyword_vector.end()){
            index = rand()%(this->number_of_keywords);
            keyword = this->keywords[index];
        }
        keyword_vector.push_back(keyword);
    }
    return keyword_vector;
}

void Database::add(string keyword,string value){//add keyword-value pair to the database
    string keyword_upper = toUpper(keyword);
    string value_upper = toUpper(value);
    if((keyword.size()!=this->keyword_hex_length) or (value.size()!=this->value_hex_length)){
        cout<<"Error in adding a new keyword-value pair in plaintext db: Keyword or value length is not match."<<endl;
        return;
    }
    auto it = find(this->keywords.begin(),this->keywords.end(),keyword_upper);
    if(it != this->keywords.end()){
        uint64_t pos_ = distance(this->keywords.begin(),it);
        this->keywords[pos_] = keyword_upper;
        this->values[pos_] = value_upper;
    }
    else{
        this->keywords.push_back(keyword_upper);
        this->values.push_back(value_upper);
        this->number_of_keywords += 1;
    }
    this->keywords_values_map[keyword_upper] = value_upper;
}

void Database::del(string keyword){//delete keyword-value pair from the database
    string keyword_upper = toUpper(keyword);
    auto it = find(this->keywords.begin(),this->keywords.end(),keyword_upper);
    if(it != this->keywords.end()){
        uint64_t pos_ = distance(this->keywords.begin(),it);
        this->keywords.erase(this->keywords.begin()+pos_);
        this->values.erase(this->values.begin()+pos_);
        this->keywords_values_map.erase(keyword_upper);
        this->number_of_keywords -= 1;
    }
}

pair<string,string> Database::generate_new_kv_pair(){//generate a new keyword-value pair
    string temp_string = generate_random_hex_string(this->keyword_hex_length);
    while (this->keywords_values_map.find(temp_string)!=this->keywords_values_map.end()){
        temp_string = generate_random_hex_string(this->keyword_hex_length);
    }
    string value = generate_random_hex_string(this->value_hex_length);
    return make_pair(temp_string,value);
}
void Database::generate_back_up_dataset(uint64_t number){
    for(uint64_t i=0;i<number;i++){
        auto [keyword,value] = generate_new_kv_pair();
        while(this->keywords_values_map_backup.find(keyword)!=this->keywords_values_map_backup.end()){
            auto k_v = generate_new_kv_pair();
            keyword = k_v.first;
            value = k_v.second;
        }
        this->keywords_values_map_backup[keyword] = value;
    }
}
pair<string,string> Database::get_back_up_k_v_pair(){
    auto it = this->keywords_values_map_backup.begin();
    string keyword = it->first;
    string value = this->keywords_values_map_backup[keyword];
    this->keywords_values_map_backup.erase(keyword);
    return make_pair(keyword,value);
}

void Database::clear(){//clear the database
    this->keywords.clear();
    this->values.clear();
    this->keywords_values_map.clear();
    keywords_values_map_backup.clear();
    this->number_of_keywords = 0;
}


IndexDatabase::IndexDatabase(uint64_t number_of_values,uint64_t value_bit_length){
    this->number_of_values = number_of_values;
    this->value_bit_length = value_bit_length;
    this->value_hex_length = ceil(float(value_bit_length)/4);
    
    srand((unsigned)time(NULL)); //set seed for random generator
    
    cout<< "Generate Random Values;" << endl;
    for(int i=0;i<this->number_of_values;i++){
        string temp_string = generate_random_hex_string(this->value_hex_length);
        this->values.push_back(temp_string);
    }
}

string IndexDatabase::get_value_from_index(uint64_t index){
    return this->values[index];
}

uint64_t IndexDatabase::get_random_index(){
    return rand()%(this->number_of_values); 
}

vector<uint64_t> IndexDatabase::get_multi_random_index(uint64_t number){
    set<uint64_t> index_set;
    for(uint64_t i=0;i<number;i++){
        uint64_t temp_index = this->get_random_index();
        if(index_set.find(temp_index)==index_set.end()){
            index_set.insert(temp_index);
        }
    }
    vector<uint64_t> index_vector;
    copy(index_set.begin(), index_set.end(), back_inserter(index_vector));
    return index_vector;
}

uint64_t max_(uint64_t a,uint64_t b){
    if(a>b){
        return a;
    }
    else{
        return b;
    }
}
uint64_t max_vector(vector<uint64_t> A){
    uint64_t max_a=A[0];
    for(uint64_t i=1;i<A.size();i++){
        if(max_a<A[i]){
            max_a = A[i];
        }
    }
    return max_a;
}

string generate_random_string(uint64_t length){
    //returns random string with length 'length'
    //this first generate random hex string, then covert it to char string
    //TODO:directly generate char string
    static const char hex_letters[] = "0123456789ABCDEF";
    string s(length*2,' ');
    for(uint64_t i=0;i<length;i++){
        s[i]=hex_letters[rand()%16];
    }
    
    string newString;
    for(uint64_t i=0; i< length; i+=2){
        string byte = s.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), 0, 16);
        newString.push_back(chr);
    }
    return newString;
}

string generate_random_hex_string(uint64_t length){
    //returns random string with length 'length'
    //this first generate random hex string
    static const char hex_letters[] = "0123456789ABCDEF";
    string s(length,' ');
    for(uint64_t i=0;i<length;i++){
        s[i]=hex_letters[rand()%16];
        //s[i]=hex_letters[i%16];
    }

    return s;
}
vector<string> divide_hex_string(string to_divide_string,uint64_t size){
    vector<string> s;
    for(uint64_t i=0; i< to_divide_string.length(); i+=size){
        s.push_back(to_divide_string.substr(i,size));
    }
    return s;
}


string hex_vector_to_poly_string(vector<string> coeffs){
    string ans = coeffs[0];
    for (int i=1;i<coeffs.size();i++){
        ans = coeffs[i] + "x^" + to_string(i) + " + " + ans;
    }
    return ans;
}
vector<uint64_t> hex_vector_to_int_vec(vector<string> coeffs,uint64_t size){
    vector<uint64_t> ans;
    for (uint64_t i=0;i<coeffs.size();i++){
        ans.push_back(stoi(coeffs[i],0,16));
    }
    for (uint64_t i=0;i<size-coeffs.size();i++){
        ans.push_back(0);
    }
    return ans;
}
vector<uint64_t> hex_vector_to_int_vec( vector<string> coeffs){
    vector<uint64_t> ans;
    for (uint64_t i=0;i<coeffs.size();i++){
        ans.push_back(stoi(coeffs[i],0,16));
    }
    return ans;
}

std::string hexStringToIntString(const std::string& hexString) {
    std::stringstream ss;
    ss << std::hex << hexString;
    int intValue;
    ss >> intValue;
    return std::to_string(intValue);
}
std::string intStringToHexString(const std::string& intString) {
    //turn dec string to hex string
    std::stringstream ss;
    ss << std::hex << std::stoi(intString);
    return ss.str();
}

std::string stringToHex(const std::string& input) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char c : input) {
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}

std::string hexToString(const std::string& input) {
    //combine two hex letter to a char
    std::string output;
    for (size_t i = 0; i < input.length(); i += 2) {
        std::string byteString = input.substr(i, 2);
        char byte = static_cast<char>(std::strtol(byteString.c_str(), nullptr, 16));
        output.push_back(byte);
    }
    return output;
}

std::string toLower(const std::string& str) {
    //turn a string to lower case
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

std::string toUpper(const std::string& str) {
    //turn a string to lower case
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}













// this code from constant pir

pair<uint64_t, string> monomial_to_hex(string monomial){
    uint64_t index;
    string coeff;
    if (monomial.find("x^") != string::npos){
        size_t hit = monomial.find("x^");
        index=stoi(monomial.substr(hit+2));
        coeff = monomial.substr(0, hit);
    } else if (monomial.find("x") != string::npos){
        size_t hit = monomial.find("x");
        index=1;
        coeff = monomial.substr(0, hit);
    } else{
        index=0;
        coeff=monomial;
    }
    return pair<uint64_t, string>(index, coeff);
}
uint64_t hex_string_to_uint(string str){
    uint64_t x;   
    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;
    return x;
}

vector<string> poly_string_to_hex_vector(std::string poly_string, uint64_t max_size){
    string __temp = poly_string;
    vector<string> ans(max_size);
    pair<uint64_t, string> mono;
    while (__temp.find("+") != string::npos){
        mono = monomial_to_hex(__temp.substr(0, __temp.find("+")));
        ans[mono.first] = mono.second;
        __temp = __temp.substr(__temp.find("+")+1);
    }
    mono = monomial_to_hex(__temp);
    ans[mono.first] = mono.second;
    return ans;
}

vector<string> uint_vector_to_hex_vector(vector<uint64_t> int_vec){
    vector<string> results;
    for(uint64_t i=0;i<int_vec.size();i++){
        std::ostringstream ss;
        ss << std::hex << i;
        std::string result = ss.str();
        results.push_back(result);
    }
    return results;
}


void print_parameters(const seal::SEALContext &context)
{
    auto &context_data = *context.key_context_data();

    /*
    Which scheme are we using?
    */
    std::string scheme_name;
    switch (context_data.parms().scheme())
    {
    case seal::scheme_type::bfv:
        scheme_name = "BFV";
        break;
    case seal::scheme_type::ckks:
        scheme_name = "CKKS";
        break;
    case seal::scheme_type::bgv:
        scheme_name = "BGV";
        break;
    default:
        throw std::invalid_argument("unsupported scheme");
    }
    std::cout << "/" << std::endl;
    std::cout << "| Encryption parameters :" << std::endl;
    std::cout << "|   scheme: " << scheme_name << std::endl;
    std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

    /*
    Print the size of the true (product) coefficient modulus.
    */
    std::cout << "|   coeff_modulus size: ";
    std::cout << context_data.total_coeff_modulus_bit_count() << " (";
    auto coeff_modulus = context_data.parms().coeff_modulus();
    std::size_t coeff_modulus_size = coeff_modulus.size();
    for (std::size_t i = 0; i < coeff_modulus_size - 1; i++)
    {
        std::cout << coeff_modulus[i].bit_count() << " + ";
    }
    std::cout << coeff_modulus.back().bit_count();
    std::cout << ") bits" << std::endl;

    /*
    For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == seal::scheme_type::bfv)
    {
        std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
    }

    std::cout << "\\" << std::endl;
}





// 计算SHA-256哈希值并输出为无符号整数
uint64_t calculateSHA256(uint64_t input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &input, sizeof(input));
    SHA256_Final(hash, &sha256);

    // 将哈希值转换为无符号整数
    uint64_t hashInt = 0;
    for (int i = 0; i < sizeof(uint64_t) && i < SHA256_DIGEST_LENGTH; i++) {
        hashInt = (hashInt << 8) | hash[i];
    }

    return hashInt;
}

std::string calculateSHA256_string(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.length());
    SHA256_Final(hash, &sha256);

    std::string hashStr;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hashStr += hash[i];
    }

    return hashStr;
}

uint64_t hash_to_bucket(uint64_t id, uint64_t count, uint64_t bucket_number,hash<uint64_t> hash_fuc){
    //return (id*5743578+count*8327498) % bucket_number;
    return (calculateSHA256(id*(1<<30)+count)) % bucket_number;
}


uint64_t hash_to_bucket_keyword(string id, uint64_t count, uint64_t bucket_number){//take upper case of the keyword


    std::string input = toUpper(id) + to_string(count);
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // 计算SHA256哈希值
    SHA256((const unsigned char*)input.c_str(), input.length(), hash);

    // 将哈希值转换为无符号整数
    unsigned int hashValue = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hashValue = (hashValue << 8) + hash[i];
    }

    return hashValue%bucket_number;
}






uint64_t choose(uint64_t n, uint64_t k) {
    if (k > n) {
        return 0;
    }
    uint64_t r = 1;
    for (uint64_t d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
vector<bool> get_perfect_constant_weight_codeword(uint64_t __number, uint64_t encoding_size, uint64_t hamming_weight){
    //For input number, output its constant-weight code with length m with k ones. (From paper "constant-weight code")
    vector<bool> ans(encoding_size, false);
    uint64_t mod_size = choose(encoding_size, hamming_weight);
    if (__number >= mod_size){
        cout << "Overflow occurred, everything okay?" << endl;
        __number %= mod_size;
    }
    long remainder = __number, k_prime = hamming_weight;
    for (long pointer=encoding_size-1; pointer>=0; pointer--){
        if (remainder >= choose(pointer, k_prime)){
            ans[pointer] = true;
            remainder -= choose(pointer, k_prime);
            k_prime -= 1;
        }
    }
    return ans;
}
uint64_t get_cw_length(uint64_t number,uint64_t k){
    //get the constant weight code length form the largest number to be coded and the hamming weight k.
    uint64_t length = k;
    while(choose(length,k)<number){
        length+=1;
    }
    return length;
}

vector<uint64_t> get_perfect_cw_ones_pos(uint64_t __number, uint64_t encoding_size, uint64_t hamming_weight){
    //For input number, output the position of ones of its constant-weight code ones. (From paper "constant-weight code")
    vector<uint64_t> ans;
    uint64_t mod_size = choose(encoding_size, hamming_weight);
    if (__number >= mod_size){
        cout << "Overflow occurred, everything okay?" << endl;
        __number %= mod_size;
    }
    long remainder = __number, k_prime = hamming_weight;
    for (long pointer=encoding_size-1; pointer>=0; pointer--){
        if (remainder >= choose(pointer, k_prime)){
            ans.push_back(pointer);
            remainder -= choose(pointer, k_prime);
            k_prime -= 1;
        }
    }
    return ans;
}



vector<uint64_t> get_ones_pos(vector<bool> cw_code){
    vector<uint64_t> pos;
    for(uint64_t i=0;i<cw_code.size();i++){
        if(cw_code[i]==true){
            pos.push_back(i);
        }
    }
    return pos;
}
// if(i % int((this->number_of_keywords)/100)==0){
//             printf("\r[%d%%]>[",i*100/(this->number_of_keywords));
//             for(int j=1;j<=i*20/this->number_of_keywords;j++){
// 			    cout<<"▉";
// 	    	}
//             for(int j=i*20/this->number_of_keywords+1;j<=20;j++){
// 			    cout<<" ";
// 	    	}
//             cout<<"]";
//             fflush(stdout);
//         }
