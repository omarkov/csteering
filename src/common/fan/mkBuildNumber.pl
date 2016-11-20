#!/usr/bin/perl

$str="";
open(FH,"<FANBuildNumber.cpp");
while (<FH>)
{
	$str=$str.$_ 	if not /buildnr|buildos|buildPlatform|buildNode|buildOSName/;
	next 		if not /buildnr|buildos|buildPlatform|buildNode|buildOSName/;

	if(/buildos/){
        $os = `uname`;
        chomp($os);
		($x,$ver)=split /=/;
		$str=$str."$x=\"$os\";\n";
	printf("************* FAN build OS 	  : $os\n");
	printf("*************\n");
	} elsif(/buildNode/){
		$os="soul";
		chomp($os);
		($x,$ver)=split /=/;
		$str=$str."$x=\"$os\";\n";
	printf("************* FAN build node      : $os\n");
	printf("*************\n");
	} elsif(/buildOSName/){
		# $os=`uname -o`;
		$os="irix";
		chomp($os);
		($x,$ver)=split /=/;
		$str=$str."$x=\"$os\";\n";
	printf("************* FAN build OS Name   : $os\n");
	printf("*************\n");
	} elsif(/buildPlatform/){
		# $os=`uname -i`;
		$os="mips";
		chomp($os);
		($x,$ver)=split /=/;
		$str=$str."$x=\"$os\";\n";
	printf("************* FAN build Platform  : $os\n");
	printf("*************\n");
	} elsif(/buildnr/){
		($x,$ver)=split /=/;
		$ver=~s/;//g;
		$ver++;
		$str=$str."$x=$ver;\n";
	printf("*************\n");
	printf("************* FAN build %.5d\n",$ver);
	printf("*************\n");
		}
}
close FH;
open(FH,">FANBuildNumber.cpp");
print FH $str;
close FH;
