{
  description = "Novus Nix Dev Environment";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-25.05";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      llvmPkg = pkgs.llvmPackages_21;
    in
    {
      packages.${system}.default = llvmPkg.stdenv.mkDerivation rec {
        pname = "novus";
        version = "0.18.0";
        src = ./.;

        nativeBuildInputs = [
          pkgs.git
          pkgs.cmake
          pkgs.makeWrapper
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DFETCHCONTENT_FULLY_DISCONNECTED=ON"
        ];

        cli11Src = pkgs.fetchFromGitHub {
          owner = "CLIUtils";
          repo = "CLI11";
          rev = "v1.9.1";
          sha256 = "hiYJoPR4izpEG9a13jM4J+xitMcUO9tVrC9HNDeAbEE=";
        };

        rangSrc = pkgs.fetchFromGitHub {
          owner = "agauniyal";
          repo = "rang";
          rev = "v3.1.0";
          sha256 = "3k5ERH8XDk9eIA3H06j9054joz7pPVrFquISP3pzKFk=";
        };

        preConfigure = ''
          mkdir -p build/_deps/cli11-src
          cp -r ${cli11Src}/* build/_deps/cli11-src
          chmod -R u+w build/_deps/cli11-src

          mkdir -p build/_deps/rang-src
          cp -r ${rangSrc}/* build/_deps/rang-src
          chmod -R u+w build/_deps/rang-src
        '';

        # TODO: Instead of this manual copying implement a proper install target in our CMake setup.
        installPhase = ''
          cd ../bin
          mv VERSION NOVUS_VERSION

          mkdir -p $out/bin
          cp -r * $out/bin/
        '';
      };

      devShells.${system} = rec {

        llvm = (pkgs.mkShellNoCC.override { stdenv = llvmPkg.stdenv; }) {
          packages = [
            pkgs.nixfmt-rfc-style
            pkgs.clang-tools

            llvmPkg.lld
            llvmPkg.libcxx
            llvmPkg.lldb
            llvmPkg.clang
            llvmPkg.bintools

            pkgs.cmake
            pkgs.gnumake
          ];
        };

        default = llvm;
      };
    };
}
