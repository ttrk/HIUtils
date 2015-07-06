/*
 * test_mergeCuts.C
 *
 * code to test variadic function mergeCuts2(int nCuts, ...);
 */

#include "../treeUtil.h"

#include <TString.h>

#include <iostream>

int main()
{
    TString cut1 = "pt>40";
    TString cut2 = "abs(eta) < 1.44";
    TString cut3 = "sigmaIetaIeta < 0.01";

    TString cut_test1 = mergeCuts2(1, cut1.Data());
    TString cut_test2 = mergeCuts2(2, cut1.Data(), cut2.Data());
    TString cut_test3 = mergeCuts2(3, cut1.Data(), cut2.Data(), cut3.Data());
    TString cut_test4 = mergeCuts2(4, cut1.Data(), cut2.Data(), cut3.Data());    // less arguments than the given number
    TString cut_test5 = mergeCuts2(2, cut1.Data(), cut2.Data(), cut3.Data());    // more arguments than the given number
    TString cut_test6 = mergeCuts2(5, cut1.Data(), cut2.Data(), cut3.Data());    // less arguments than the given number

    std::cout << "test1 = " << cut_test1.Data() << std::endl;
    std::cout << "test2 = " << cut_test2.Data() << std::endl;
    std::cout << "test3 = " << cut_test3.Data() << std::endl;
    std::cout << "test4 = " << cut_test4.Data() << std::endl;
    std::cout << "test5 = " << cut_test5.Data() << std::endl;
    std::cout << "test6 = " << cut_test6.Data() << std::endl;
}
