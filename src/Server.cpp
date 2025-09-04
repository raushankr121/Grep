#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <optional>
#include <unordered_map>
#include <unordered_set>

void print_line(const std::string &str)
{
    std::cout << str << std::endl;
}

struct MatchContext
{
    std::unordered_map<size_t, std::pair<size_t, size_t>> found_groups;
    size_t next_group_index = 1;

    void add_group(std::pair<size_t, size_t> coords)
    {
        found_groups[next_group_index++] = coords;
    }

    void reset()
    {
        found_groups.clear();
        next_group_index = 1;
    }
};

std::optional<size_t> match_group(const char *group, const char *text, size_t text_i, MatchContext &ctx);
std::optional<size_t> match_here(const char *regexp, size_t reg_i, const char *text, size_t text_i, MatchContext &ctx);

int get_group_len(const char *group)
{
    int depth = 1;
    int len_of_phrase = 1;

    do
    {
        if (*(group + len_of_phrase) == '(')
        {
            depth++;
        }

        if (*(group + len_of_phrase) == ')')
        {
            depth--;
        }

        len_of_phrase++;

        /* code */
    } while (depth != 0);

    return len_of_phrase;
}

std::optional<size_t> match_group(const char *group, const char *text, size_t text_i, MatchContext &ctx)
{

    int len_of_phrase = get_group_len(group);

    std::string phrase(group + 1, group + len_of_phrase - 1);
    char poss_phrase_quantifier = *(group + len_of_phrase);

    // std::cerr << "[group pattern] phrase: " << phrase << std::endl;

    std::vector<std::string> poss_regexs;
    int depth = 1;
    std::string poss_regex;
    for (char c : phrase)
    {
        if (c == '|' && depth == 1)
        {
            // std::cout << "[curr poss regex]: " << poss_regex << std::endl;

            poss_regexs.push_back(poss_regex);
            poss_regex.clear();
            continue;
        }

        if (c == '(')
        {
            depth++;
        }

        if (c == ')')
        {
            depth--;
        }

        poss_regex += c;
    }
    poss_regexs.push_back(poss_regex);

    for (std::string s : poss_regexs)
    {
        // std::cerr << s << std::endl;

        if (auto result = match_here(s.c_str(), 0, text, text_i, ctx))
        {
            // std::cerr << "ADDING HERE ----" << std::endl;
            ctx.add_group({text_i, result.value()});
            return result;
        }
    }

    return std::nullopt;
}

std::optional<size_t> match_here(const char *regexp, size_t reg_i, const char *text, size_t text_i, MatchContext &ctx)
{

    const char *curr_regexp = regexp + reg_i;
    const char *curr_text = text + text_i;

    // std::cerr << "[match_here] regex: \"" << curr_regexp << "\", text: \"" << curr_text << "\"" << std::endl;

    if (*(curr_regexp) == '\0')
    {
        return text_i;
    }

    if (*(curr_regexp) == '$' && *(curr_regexp + 1) == '\0')
    {
        if (*(curr_text) == '\0')
        {
            return text_i;
        }

        return std::nullopt;
    }
    if (*curr_regexp == '(')
    {

        size_t group_len = get_group_len(curr_regexp);
        const char poss_group_quantifier = *(curr_regexp + group_len);
        // std::cerr << "[POSS QUANT]: " << poss_group_quantifier << std::endl;

        if (poss_group_quantifier == '+')
        {
            std::vector<size_t> match_positions;

            // Match at least once
            if (auto first_match = match_group(curr_regexp, text, text_i, ctx))
            {
                size_t next_text_i = first_match.value();
                match_positions.push_back(next_text_i);

                // Try to match multiple times (greedy)
                while (auto next_match = match_group(curr_regexp, text, match_positions.back(), ctx))
                {
                    match_positions.push_back(next_match.value());
                }

                // Try from most greedy down to least
                for (auto it = match_positions.rbegin(); it != match_positions.rend(); ++it)
                {
                    size_t next_reg_i = reg_i + group_len + 1; // past group + quantifier
                    if (auto result = match_here(regexp, next_reg_i, text, *it, ctx))
                    {
                        return result;
                    }
                }

                return std::nullopt;
            }

            // Group didn't match even once — fail
            return std::nullopt;
        }
        if (poss_group_quantifier == '?')
        {
            if (auto result = match_group(curr_regexp, text, text_i, ctx))
            {
                size_t chars_consumed = result.value() - text_i;
                // std::cout << "[GROUP CONSUMED]: " << chars_consumed << std::endl;
                size_t next_reg_i = reg_i + group_len + 1;
                if (auto greedy = match_here(regexp, next_reg_i, text, text_i + chars_consumed, ctx))
                {
                    return greedy;
                }
            }

            return match_here(curr_regexp, reg_i + group_len + 1, text, text_i, ctx);
        }

        if (auto result = match_group(curr_regexp, text, text_i, ctx))
        {

            size_t chars_consumed = result.value() - text_i;
            // std::cout << "[GROUP CONSUMED]: " << chars_consumed << std::endl;
            size_t next_reg_i = reg_i + group_len;

            if (auto after_group_match = match_here(regexp, next_reg_i, text, text_i + chars_consumed, ctx))
            {
                return after_group_match;
            }
        }

        return std::nullopt;
    }
    if (*(curr_regexp + 1) == '?')
    {

        char char_to_find = *curr_regexp;
        // std::cerr << "[? quantifier] regex: \"" << regexp << "\", text: \"" << text << "\"" << std::endl;

        // Try to match one character (greedy)
        if (char_to_find == *curr_text || char_to_find == '.')
        {
            if (auto result = match_here(regexp, reg_i + 2, text, text_i + 1, ctx))
            {
                return result;
            }
        }

        // try no match
        return match_here(regexp, reg_i + 2, text, text_i, ctx);
    }
    if (*(curr_regexp + 1) == '+')
    {

        // Must match at least once
        if (*curr_text != *curr_regexp && *curr_regexp != '.')
        {
            return std::nullopt;
        }

        size_t start = text_i;
        size_t end = text_i;

        while (text[end] != '\0' && (text[end] == *curr_regexp || *curr_regexp == '.'))
        {
            end++;
        }

        for (size_t i = end; i > start; --i)
        {
            if (auto result = match_here(regexp, reg_i + 2, text, i, ctx))
            {
                return result;
            }
        }

        return std::nullopt;
    }
    if (*curr_regexp == '.' || *curr_regexp == *curr_text)
    {
        return match_here(regexp, reg_i + 1, text, text_i + 1, ctx);
    }
    if (*curr_regexp == '\\')
    {
        const char poss_quantifier = *(curr_regexp + 2);

        if (*(curr_regexp + 1) == 'd')
        {

            if (poss_quantifier == '+')
            {

                if (!(isdigit(*curr_text)))
                {
                    return std::nullopt;
                }

                size_t start = text_i;
                size_t end = text_i;

                while (text[end] != '\0' && (isdigit(text[end])))
                {
                    end++;
                }

                for (size_t i = end; i > start; --i)
                {
                    if (auto result = match_here(regexp, reg_i + 3, text, i, ctx))
                    {
                        return result;
                    }
                }

                return std::nullopt;
            }

            if (isdigit(*curr_text))
            {
                return match_here(regexp, reg_i + 2, text, text_i + 1, ctx);
            }
        }

        if (*(curr_regexp + 1) == 'w')
        {

            if (poss_quantifier == '+')
            {

                if (!(isalnum(*curr_text) || *curr_text == '_'))
                {
                    return std::nullopt;
                }

                size_t start = text_i;
                size_t end = text_i;

                while (text[end] != '\0' && (isalnum(text[end]) || text[end] == '_'))
                {
                    end++;
                }

                for (size_t i = end; i > start; --i)
                {
                    if (auto result = match_here(regexp, reg_i + 3, text, i, ctx))
                    {
                        return result;
                    }
                }

                return std::nullopt;
            }

            if (isalnum(*curr_text) || *curr_text == '_')
            {
                return match_here(regexp, reg_i + 2, text, text_i + 1, ctx);
            }
        }

        if (isdigit(*(curr_regexp + 1)))
        {
            // for (const auto &pair : ctx.found_groups)
            // {
            //     std::string txt_match(text + pair.second.first, pair.second.second - pair.second.first);
            //     std::cout << "Key: " << pair.first << ", Value: " << txt_match << std::endl;
            // }

            // char '1' is '49' in ASCII, char '0' is 49,
            //  to get 1 = 49 - 48 = *(curr_regexp + 1) - '0'
            size_t backref_idx = *(curr_regexp + 1) - '0';

            if (ctx.found_groups.count(backref_idx))
            {
                std::pair<size_t, size_t> coords = ctx.found_groups.at(backref_idx);
                std::string found_match(text + coords.first, coords.second - coords.first);

                if (auto result = match_here(found_match.c_str(), 0, text, text_i, ctx))
                {
                    return match_here(regexp, reg_i + 2, text, text_i + found_match.size(), ctx);
                }
            }
        }
    }

    if (*curr_regexp == '[')
    {

        std::unordered_set<char> char_group;
        int phrase_len = 1;

        while (*(curr_regexp + phrase_len) != ']')
        {
            if (*(curr_regexp + phrase_len) != '^')
            {
                char_group.insert(*(curr_regexp + phrase_len));
            }
            phrase_len++;
        }

        // for(char c: char_group){
        //     std::cout << "CHAR IN SET: " << c << std::endl;
        // }

        // std::cout << char_group.size() << std::endl;

        const char poss_group_quantifier = *(curr_regexp + phrase_len + 1);
        int negative_enabled = *(curr_regexp + 1) == '^' ? true : false;

        if (poss_group_quantifier == '+')
        {

            if (negative_enabled)
            {

                if (*curr_text == '\0')
                {
                    return std::nullopt;
                }

                if (char_group.count(*(curr_text)))
                {
                    return std::nullopt;
                }

                size_t start = text_i;
                size_t end = text_i;

                while (text[end] != '\0' && !char_group.count(text[end]))
                {
                    end++;
                }

                for (size_t i = end; i > start; --i)
                {
                    if (auto result = match_here(regexp, reg_i + phrase_len + 2, text, i, ctx))
                    {
                        return result;
                    }
                }

                return std::nullopt;
            }

            if (!char_group.count(*(curr_text)))
            {
                return std::nullopt;
            }

            size_t start = text_i;
            size_t end = text_i;

            while (text[end] != '\0' && char_group.count(text[end]))
            {
                end++;
            }

            for (size_t i = end; i > start; --i)
            {
                if (auto result = match_here(regexp, reg_i + phrase_len + 2, text, i, ctx))
                {
                    return result;
                }
            }

            return std::nullopt;
        }

        if (negative_enabled)
        {
            if (*curr_text == '\0')
            {
                return std::nullopt;
            }

            // std::cout << "IN NEGATIVE" << std::endl;
            // std::cout << *curr_text << std::endl;
            // Character is in the excluded set → do not match
            if (char_group.count(*curr_text))
            {
                // print_line("returning null");
                return std::nullopt;
            }

            // Character not in excluded set → match and advance
            return match_here(regexp, reg_i + phrase_len + 1, text, text_i + 1, ctx);
        }

        if (char_group.count(*curr_text))
        {
            return match_here(regexp, reg_i + phrase_len + 1, text, text_i + 1, ctx);
        }

        return std::nullopt; // No match found in the character class
    }
    // if (*regexp == '*') {
    //     return match_here(regexp + 1, text) || (*text != '\0' && match_here(regexp, text + 1));
    // }

    return std::nullopt;
}

bool match_pattern(const std::string &input_line, const std::string &pattern)
{

    const char *regexp = pattern.c_str();
    const char *text = input_line.c_str();

    MatchContext ctx;

    if (*regexp == '^')
    {
        auto result = match_here(regexp, 1, text, 0, ctx);
        if (result)
        {
            // std::cout << "[CONSUMED]: " << result.value() << std::endl;
            return true;
        }
    }
    else
    {
        do
        {
            auto result = match_here(regexp, 0, text, 0, ctx);
            if (result)
            {
                // std::cout << "[CONSUMED]: " << result.value() << std::endl;
                return true;
            }
            /* code */
        } while (*text++ != '\0');
    }

    return false;
}

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here" << std::endl;

    if (argc != 3)
    {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E")
    {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    // Uncomment this block to pass the first stage

    std::string input_line;
    std::getline(std::cin, input_line);

    try
    {
        if (match_pattern(input_line, pattern))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}