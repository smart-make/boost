/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "files.hpp"
#include <boost/filesystem/fstream.hpp>
#include <boost/unordered_map.hpp>
#include <boost/range/algorithm/upper_bound.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iterator>

namespace quickbook
{
    namespace
    {
        boost::unordered_map<fs::path, file_ptr> files;
    }

    // Read the first few bytes in a file to see it starts with a byte order
    // mark. If it doesn't, then write the characters we've already read in.
    // Although, given how UTF-8 works, if we've read anything in, the files
    // probably broken.

    template <typename InputIterator, typename OutputIterator>
    bool check_bom(InputIterator& begin, InputIterator end,
            OutputIterator out, char const* chars, int length)
    {
        char const* ptr = chars;

        while(begin != end && *begin == *ptr) {
            ++begin;
            ++ptr;
            --length;
            if(length == 0) return true;
        }

        // Failed to match, so write the skipped characters to storage:
        while(chars != ptr) *out++ = *chars++;

        return false;
    }

    template <typename InputIterator, typename OutputIterator>
    std::string read_bom(InputIterator& begin, InputIterator end,
            OutputIterator out)
    {
        if(begin == end) return "";

        const char* utf8 = "\xef\xbb\xbf" ;
        const char* utf32be = "\0\0\xfe\xff";
        const char* utf32le = "\xff\xfe\0\0";

        unsigned char c = *begin;
        switch(c)
        {
        case 0xEF: { // UTF-8
            return check_bom(begin, end, out, utf8, 3) ? "UTF-8" : "";
        }
        case 0xFF: // UTF-16/UTF-32 little endian
            return !check_bom(begin, end, out, utf32le, 2) ? "" :
                check_bom(begin, end, out, utf32le + 2, 2) ? "UTF-32" : "UTF-16";
        case 0: // UTF-32 big endian
            return check_bom(begin, end, out, utf32be, 4) ? "UTF-32" : "";
        case 0xFE: // UTF-16 big endian
            return check_bom(begin, end, out, utf32be + 2, 2) ? "UTF-16" : "";
        default:
            return "";
        }
    }

    // Copy a string, converting mac and windows style newlines to unix
    // newlines.

    template <typename InputIterator, typename OutputIterator>
    void normalize(InputIterator begin, InputIterator end,
            OutputIterator out)
    {
        std::string encoding = read_bom(begin, end, out);

        if(encoding != "UTF-8" && encoding != "")
        throw load_error(encoding +
            " is not supported. Please use UTF-8.");

        while(begin != end) {
            if(*begin == '\r') {
                *out++ = '\n';
                ++begin;
                if(begin != end && *begin == '\n') ++begin;
            }
            else {
                *out++ = *begin++;
            }
        }
    }

    file_ptr load(fs::path const& filename, unsigned qbk_version)
    {
        boost::unordered_map<fs::path, file_ptr>::iterator pos
            = files.find(filename);

        if (pos == files.end())
        {
            fs::ifstream in(filename, std::ios_base::in);

            if (!in)
                throw load_error("Could not open input file.");

            // Turn off white space skipping on the stream
            in.unsetf(std::ios::skipws);

            std::string source;
            normalize(
                std::istream_iterator<char>(in),
                std::istream_iterator<char>(),
                std::back_inserter(source));

            if (in.bad())
                throw load_error("Error reading input file.");

            bool inserted;

            boost::tie(pos, inserted) = files.emplace(
                filename, new file(filename, source, qbk_version));

            assert(inserted);
        }

        return pos->second;
    }

    std::ostream& operator<<(std::ostream& out, file_position const& x)
    {
        return out << "line: " << x.line << ", column: " << x.column;
    }

    file_position relative_position(
        boost::string_ref::const_iterator begin,
        boost::string_ref::const_iterator iterator)
    {
        file_position pos;
        boost::string_ref::const_iterator line_begin = begin;

        while (begin != iterator)
        {
            if (*begin == '\r')
            {
                ++begin;
                ++pos.line;
                line_begin = begin;
            }
            else if (*begin == '\n')
            {
                ++begin;
                ++pos.line;
                line_begin = begin;
                if (begin == iterator) break;
                if (*begin == '\r')
                {
                    ++begin;
                    line_begin = begin;
                }
            }
            else
            {
                ++begin;
            }
        }

        pos.column = iterator - line_begin + 1;
        return pos;
    }

    file_position file::position_of(boost::string_ref::const_iterator iterator) const
    {
        return relative_position(source().begin(), iterator);
    }

    // Mapped files.

    struct mapped_file_section
    {
        enum section_types {
            normal,
            empty,
            indented
        };
    
        std::string::size_type original_pos;
        std::string::size_type our_pos;
        section_types section_type;
        int indentation;

        mapped_file_section(
                std::string::size_type original_pos,
                std::string::size_type our_pos,
                section_types section_type = normal,
                int indentation = 0) :
            original_pos(original_pos), our_pos(our_pos),
            section_type(section_type), indentation(indentation) {}
    };

    struct mapped_section_original_cmp
    {
        bool operator()(mapped_file_section const& x,
                mapped_file_section const& y)
        {
            return x.original_pos < y.original_pos;
        }

        bool operator()(mapped_file_section const& x,
                std::string::size_type const& y)
        {
            return x.original_pos < y;
        }

        bool operator()(std::string::size_type const& x,
                mapped_file_section const& y)
        {
            return x < y.original_pos;
        }
    };

    struct mapped_section_pos_cmp
    {
        bool operator()(mapped_file_section const& x,
                mapped_file_section const& y)
        {
            return x.our_pos < y.our_pos;
        }

        bool operator()(mapped_file_section const& x,
                std::string::size_type const& y)
        {
            return x.our_pos < y;
        }

        bool operator()(std::string::size_type const& x,
                mapped_file_section const& y)
        {
            return x < y.our_pos;
        }
    };
    
    struct mapped_file : file
    {
        mapped_file(file_ptr original) :
            file(*original, std::string()),
            original(original), mapped_sections()
        {}

        file_ptr original;
        std::vector<mapped_file_section> mapped_sections;
        
        void add_empty_mapped_file_section(boost::string_ref::const_iterator pos) {
            std::string::size_type original_pos =
                pos - original->source().begin();
        
            if (mapped_sections.empty() ||
                    mapped_sections.back().section_type !=
                        mapped_file_section::empty ||
                    mapped_sections.back().original_pos != original_pos)
            {
                mapped_sections.push_back(mapped_file_section(
                        original_pos, source().size(),
                        mapped_file_section::empty));
            }
        }

        void add_mapped_file_section(boost::string_ref::const_iterator pos) {
            mapped_sections.push_back(mapped_file_section(
                pos - original->source().begin(), source().size()));
        }

        void add_indented_mapped_file_section(boost::string_ref::const_iterator pos,
                int indentation)
        {
            mapped_sections.push_back(mapped_file_section(
                pos - original->source().begin(), source().size(),
                mapped_file_section::indented, indentation));
        }

        std::string::size_type to_original_pos(
            std::vector<mapped_file_section>::const_iterator section,
            std::string::size_type pos) const
        {
            switch (section->section_type) {
                case mapped_file_section::normal:
                    return pos - section->our_pos + section->original_pos;
                case mapped_file_section::empty:
                    return section->original_pos;
                case mapped_file_section::indented: {
                    boost::string_ref::size_type our_line = section->our_pos;
                    unsigned newline_count = 0;

                    for(boost::string_ref::size_type i = section->our_pos;
                        i != pos; ++i)
                    {
                        if (source()[i] == '\n') {
                            our_line = i + 1;
                            ++newline_count;
                        }
                    }
                    
                    if (newline_count == 0)
                        return pos - section->our_pos + section->original_pos;

                    boost::string_ref::size_type original_line =
                        section->original_pos;
                    
                    while(newline_count > 0) {
                        if (original->source()[original_line] == '\n')
                            --newline_count;
                        ++original_line;
                    }
                    
                    for(unsigned i = section->indentation; i > 0; --i) {
                        if (original->source()[original_line] == '\n' ||
                            original->source()[original_line] == '\0') break;
                        assert(original->source()[original_line] == ' ' ||
                            original->source()[original_line] == '\t');
                        ++original_line;
                    }
                    
                    assert(original->source()[original_line] ==
                        source()[our_line]);

                    return original_line + (pos - our_line);
                }
                default:
                    assert(false);
                    return section->original_pos;
            }
        }
        
        std::vector<mapped_file_section>::const_iterator find_section(
            boost::string_ref::const_iterator pos) const
        {
            std::vector<mapped_file_section>::const_iterator section =
                boost::upper_bound(mapped_sections,
                    std::string::size_type(pos - source().begin()),
                    mapped_section_pos_cmp());
            assert(section != mapped_sections.begin());
            --section;

            return section;
        }

        virtual file_position position_of(boost::string_ref::const_iterator) const;
    };

    namespace {
        std::list<mapped_file> mapped_files;
    }

    struct mapped_file_builder_data
    {
        mapped_file_builder_data() { reset(); }
        void reset() { new_file.reset(); }
    
        boost::intrusive_ptr<mapped_file> new_file;
    };

    mapped_file_builder::mapped_file_builder() : data(0) {}
    mapped_file_builder::~mapped_file_builder() { delete data; }

    void mapped_file_builder::start(file_ptr f)
    {
        if (!data) {
            data = new mapped_file_builder_data;
        }

        assert(!data->new_file);
        data->new_file = new mapped_file(f);
    }

    file_ptr mapped_file_builder::release()
    {
        file_ptr r = data->new_file;
        data->reset();
        return r;
    }

    void mapped_file_builder::clear()
    {
        data->reset();
    }
    
    bool mapped_file_builder::empty() const
    {
        return data->new_file->source().empty();
    }

    mapped_file_builder::pos mapped_file_builder::get_pos() const
    {
        return data->new_file->source().size();
    }
    
    void mapped_file_builder::add_at_pos(boost::string_ref x, iterator pos)
    {
        data->new_file->add_empty_mapped_file_section(pos);
        data->new_file->source_.append(x.begin(), x.end());
    }

    void mapped_file_builder::add(boost::string_ref x)
    {
        data->new_file->add_mapped_file_section(x.begin());
        data->new_file->source_.append(x.begin(), x.end());
    }

    void mapped_file_builder::add(mapped_file_builder const& x)
    {
        add(x, 0, x.data->new_file->source_.size());
    }

    void mapped_file_builder::add(mapped_file_builder const& x,
            pos begin, pos end)
    {
        assert(data->new_file->original == x.data->new_file->original);
        assert(begin <= x.data->new_file->source_.size());
        assert(end <= x.data->new_file->source_.size());

        if (begin != end)
        {
            std::vector<mapped_file_section>::const_iterator start =
                x.data->new_file->find_section(
                    x.data->new_file->source().begin() + begin);
    
            std::string::size_type size = data->new_file->source_.size();
    
            data->new_file->mapped_sections.push_back(mapped_file_section(
                    x.data->new_file->to_original_pos(start, begin),
                    size, start->section_type, start->indentation));
    
            for (++start; start != x.data->new_file->mapped_sections.end() &&
                    start->our_pos < end; ++start)
            {
                data->new_file->mapped_sections.push_back(mapped_file_section(
                    start->original_pos, start->our_pos - begin + size,
                    start->section_type, start->indentation));
            }
    
            data->new_file->source_.append(
                x.data->new_file->source_.begin() + begin,
            x.data->new_file->source_.begin() + end);
        }
    }

    void mapped_file_builder::unindent_and_add(boost::string_ref x)
    {
        std::string program(x.begin(), x.end());

        // Erase leading blank lines and newlines:
        std::string::size_type start = program.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return;

        start = program.find_last_of("\r\n", start);
        if (start != std::string::npos)
        {
            ++start;
            program.erase(0, start);
        }

        assert(program.size() != 0);

        // Get the first line indent
        std::string::size_type indent = program.find_first_not_of(" \t");
        std::string::size_type pos = 0;
        if (std::string::npos == indent)
        {
            // Nothing left to do here. The code is empty (just spaces).
            // We clear the program to signal the caller that it is empty
            // and return early.
            program.clear();
            return;
        }

        // Calculate the minimum indent from the rest of the lines
        do
        {
            pos = program.find_first_not_of("\r\n", pos);
            if (std::string::npos == pos)
                break;

            std::string::size_type n = program.find_first_not_of(" \t", pos);
            if (n != std::string::npos)
            {
                char ch = program[n];
                if (ch != '\r' && ch != '\n') // ignore empty lines
                    indent = (std::min)(indent, n-pos);
            }
        }
        while (std::string::npos != (pos = program.find_first_of("\r\n", pos)));

        // Trim white spaces from column 0..indent
        pos = 0;
        program.erase(0, indent);
        while (std::string::npos != (pos = program.find_first_of("\r\n", pos)))
        {
            if (std::string::npos == (pos = program.find_first_not_of("\r\n", pos)))
            {
                break;
            }

            std::string::size_type next = program.find_first_of("\r\n", pos);
            program.erase(pos, (std::min)(indent, next-pos));
        }

        data->new_file->add_indented_mapped_file_section(x.begin() + indent, indent);
        data->new_file->source_.append(program);
    }

    file_position mapped_file::position_of(boost::string_ref::const_iterator pos) const
    {
        return original->position_of(original->source().begin() +
            to_original_pos(find_section(pos), pos - source().begin()));
    }
}
