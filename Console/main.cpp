#include <iostream>
using namespace std;
#include "arg_parser.h"
#include "../Engine/Engine.h"

int main(int argc, char *argv[])
{
    Engine engine;
    typedef pair<char, const char *> command;
    string sourcePath, resultPath, errors;
    vector< command > commands;

    const Arg_parser::Option Options[] =
    {
        { 'x', "grayscale",   Arg_parser::no    },
        { 'g', "gaussian",    Arg_parser::yes    },
        { 'a', "grayworld",    Arg_parser::no    },
        { 'm', "median", Arg_parser::yes},
        { 's', "sobel", Arg_parser::no },
        { 'c', "autocontrast", Arg_parser::no },
        { 'l', "autolevels", Arg_parser::no },
        { 'r', "rotate", Arg_parser::yes },
        { 'z', "resize", Arg_parser::yes },
        { 'k', "custom", Arg_parser::yes }
    };

    const Arg_parser ArgParser( argc, argv, Options );
    if( ArgParser.error().size())
    {
        cerr<<"Command line error: "<<ArgParser.error()<<endl;
        return -1;
    }

    for (uint k = 0; k < ArgParser.arguments(); ++k)
    {
        int iCode = ArgParser.code(k);

        if (iCode != 0)
        {
            commands.push_back(command((char)iCode, ArgParser.argument(k).data()));
        }
        else
        {
            if (sourcePath.empty())
                sourcePath = ArgParser.argument(k);
            else if (resultPath.empty())
                resultPath = ArgParser.argument(k);
            else
            {
                cerr<<"invalid unnamed command-line argument"<<endl;
                return -1;
            }
        }
    }

    if (sourcePath.empty())
        errors += "\n* Source path was not specified.";
    if (resultPath.empty())
        errors += "\n* Result path was not specified.";
    if (!engine.load(sourcePath))
        errors += "\n* Can't load source image";

    if (!errors.empty())
    {
        cerr<<"Some errors were found:";
        cerr<<errors<<endl;
        return -1;
    }

    for (uint i = 0; i < commands.size(); ++i)
    {
        switch (commands[i].first)
        {
        case 'x':
            engine.toGrayscale();
            break;
        case 'g':
            engine.addGaussFilter(atof(commands[i].second));
            break;
        case 'a':
            engine.toGrayWorld();
            break;
        case 'm':
            engine.addMedianFilter(2 * atof(commands[i].second));
            break;
        case 's':
            engine.addSobelOne();
            break;
        case 'c':
            engine.autoContrast();
            break;
        case 'l':
            engine.autoLevels();
            break;
        case 'r':
            engine.rotate(atof(commands[i].second));
            break;
        case 'z':
            engine.scale(atof(commands[i].second));
            break;
        case 'k':
            engine.addMatrixFilterNonParsed(commands[i].second);
            break;
        default:
            cout<<"Unknown option: "<<commands[i].first<<"; its argument: "<<commands[i].second<<endl;
            break;
        }
    }

    if (!engine.save(resultPath))
    {
        cerr<<"Can't save output file"<<endl;
        return -1;
    }

    return 0;
}
