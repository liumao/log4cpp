/*
 * HierarchyMaintainer.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "log4cpp/Hints.hh"

#ifdef _MSC_VER
#    include <io.h>
#else
#    include <unistd.h>
#endif // _MSC_VER

#include <stdio.h>
#include "log4cpp/HierarchyMaintainer.hh"
#include "log4cpp/FileAppender.hh"

namespace log4cpp {
    Log4cppCleanup& HierarchyMaintainer::_fuckinDummy(Log4cppCleanup::_cleanup);

    HierarchyMaintainer* HierarchyMaintainer::_defaultMaintainer = NULL;

    HierarchyMaintainer& HierarchyMaintainer::getDefaultMaintainer() {
        if (!_defaultMaintainer)
            _defaultMaintainer = new HierarchyMaintainer();

        return *_defaultMaintainer;
    }

    HierarchyMaintainer::HierarchyMaintainer() {
    }

    HierarchyMaintainer::~HierarchyMaintainer() {
        shutdown();
        deleteAllCategories();
    }

    Category& HierarchyMaintainer::getInstance(const std::string& name) {
        CategoryMap::iterator i = _categoryMap.find(name);
        if (_categoryMap.end() == i) {
            Category* result;
            
            if (name == "") {
                result = new Category(name, NULL, Priority::INFO);
                result->setAppender(new FileAppender("_", ::dup(fileno(stderr))));
            } else {
                std::string parentName;
                size_t dotIndex = name.find_last_of('.');
                if (name.length() <= dotIndex) {
                    parentName = "";
                } else {
                    parentName = name.substr(0, dotIndex);
                }
                Category& parent = getInstance(parentName);
                result = new Category(name, &parent, Priority::NOTSET);
            }	  
            _categoryMap[name] = result; 
            return *result;
        } else {
            return *((*i).second);
        }
    }

    std::set<Category*>* HierarchyMaintainer::getCurrentCategories() const {
        std::set<Category*>* categories = new std::set<Category*>;

        for(CategoryMap::const_iterator i = _categoryMap.begin(); i != _categoryMap.end(); i++) {
            categories->insert((*i).second);
        }

        return categories;
    }

    void HierarchyMaintainer::shutdown() {
        for(CategoryMap::const_iterator i = _categoryMap.begin(); i != _categoryMap.end(); i++) {
            ((*i).second)->removeAllAppenders();
        }
    }

    void HierarchyMaintainer::deleteAllCategories() {
        for(CategoryMap::const_iterator i = _categoryMap.begin(); i != _categoryMap.end(); i++) {
            delete ((*i).second);
        }
    }

}
