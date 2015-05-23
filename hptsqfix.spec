%define reldate 20150523
%define reltype C
# may be one of: C (current), R (release), S (stable)

Name: hptsqfix
Version: 1.9.%{reldate}%{reltype}
Release: 1
Group: Applications/FTN
Summary: hptsqfix - program to rebuild index and some info in squish message bases
URL: http://huskyproject.org
License: GPL
Requires: smapi >= 2.5
BuildRequires: huskylib >= 1.9, smapi >= 2.5
Source: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
hptsqfix is a tool for fixing squish message bases from the Husky Project.

%prep
%setup -q -n %{name}

%build
make

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/*
%{_mandir}/man1/*
