Summary:            A runtime for the Cinquecento language
Name:               l1 
Version:            3.11
Release:            workgroups
License:            GPLv2+
Group:              System Environment/Base
Source:             %{name}-%{version}.tar.gz
#Patch1:             eject-2.1.1-verbose.patch
#Patch2:             eject-timeout.patch
#Patch3:             eject-2.1.5-opendevice.patch
#Patch4:             eject-2.1.5-spaces.patch
#Patch5:             eject-2.1.5-lock.patch
#Patch6:             eject-2.1.5-umount.patch
URL:                http://www.cqctworld.org
BuildRequires:      bison gcc make

%description
A runtime for the Cinquecento language

%prep
%setup -q -n %{name}

%build
make

%install
make PREFIX=%{buildroot}/usr install

%files
%doc COPYING COPYRIGHT README doc/*.html doc/*.svg doc/*.css
%{_bindir}/*
/usr/share/*

%changelog
* Sat Aug 31 2013 Cinquecento Labs
- constructed spec file
