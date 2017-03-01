#!/bin/sh
astyle --style=k\&r -l --convert-tabs --pad-header --indent-switches --align-pointer=name --indent=spaces=4 *.cpp *.h 
