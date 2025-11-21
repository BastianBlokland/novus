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
