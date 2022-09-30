/**
 * @brief 获取毫秒级时间戳
 * 
 * @return time_t 
 */
time_t clock2() {
	return chrono::duration_cast<chrono::milliseconds>
	(chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 字符串批量替换
 * 
 * @param from 查找
 * @param to 替换为
 * @param source 源字符串
 * @return string
 */
string str_replace(string from, string to, string source) {
    string result = source;
	int st = 0, wh = result.find(from.c_str(), st);
	while (wh != string::npos) {
		result.replace(wh, from.size(), to.c_str());
		st = wh + to.size();
		wh = result.find(from.c_str(), st);
	} 
    return result;
}

string quoteEncode(string quote) {
	quote = str_replace("\\", "\\\\", quote);
	quote = str_replace("\"", "\\\"", quote);
	return quote;
}