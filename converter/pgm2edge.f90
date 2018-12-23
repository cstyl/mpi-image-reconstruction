module ipgmio

implicit none

integer, parameter, private :: iounit = 10
integer, parameter :: thresh = 255

contains


!  Routine to get size of a PGM image
!  Note that this assumes no comments and no other white space
!  You can create PGMS using:
!  convert image.jpg -colorspace gray -compress none -depth 8 image.pgm 
!  then edit them slightly.

subroutine ipgmsize(filename, nx, ny)

  integer :: nx, ny

  character*(*) :: filename

  open(unit=iounit, file=filename)

  read(iounit,*)
  read(iounit,*) nx, ny

  close(unit=iounit)

end subroutine ipgmsize


subroutine ipgmread(filename, x)

  character*(*) :: filename
  integer :: nx, ny, nxt, nyt
  integer, dimension(:,:) :: x

  integer i, j

  nx = size(x,1)
  ny = size(x,2)

  write(*,*) 'Reading ', nx, ' x ', ny, ' picture from file: ', filename

  open(unit=iounit, file=filename)

  read(iounit,*)
  read(iounit,*) nxt, nyt

  if (nx .ne. nxt .or. ny .ne. nyt) then
    write(*,*) 'pgmread: size mismatch, (nx,ny) = (', nxt, ',', nyt, &
               ') expected (', nx, ',', ny, ')'
    stop
  end if

  read(iounit,*)
  read(iounit,*) ((x(i,ny-j+1), i=1,nx), j=1,ny)

  close(unit=iounit)

end subroutine ipgmread


subroutine ipgmwrite(filename, x)

  implicit none

  character*(*) :: filename
  integer, dimension(:,:) :: x

  integer :: i, j, nx, ny

  nx = size(x,1)
  ny = size(x,2)

  write(*,*) 'Writing ', nx, ' x ', ny, ' picture into file: ', filename

  open(unit=iounit, file=filename)

  write(iounit,fmt='(''P2''/''# Written by ipgmwrite'')')
  write(iounit,fmt='(i4, '' '', i4)') nx, ny

  write(iounit,fmt='(i4)') thresh
  write(iounit,fmt='(12('' '', i5))') ((x(i, ny-j+1) , i=1,nx), j=1,ny)

  close(unit=iounit)

end subroutine ipgmwrite


double precision function boundaryval(i, m)

  implicit none

  integer :: i, m
  double precision :: val

  val = 2.0*dble(i-1)/dble(m-1)
  if (i .ge. m/2+1) val = 2.0-val
  
  boundaryval = val

end function boundaryval

end module ipgmio


program pgm2edge

  use ipgmio

  implicit none

  integer, allocatable, dimension(:,:) :: image, edge

  integer, parameter :: maxlen = 32

  character*(maxlen) :: infile, outfile

  double precision :: val

  integer :: i, j, nx, ny, imax, imin

  if (command_argument_count() /= 2) then
     write(*,*) 'Usage: pgm2edge <input image file> <output edge file>'
     stop
  end if

  call get_command_argument(1, infile)
  call get_command_argument(2, outfile)

  infile  = trim(infile)
  outfile = trim(outfile)
  
  write(*,*) 'pgm2edge: infile = ',  infile, 'outfile = ', outfile

  call ipgmsize(infile, nx, ny)

  write(*,*) 'nx, ny = ', nx, ny

  allocate(image(0:nx+1,0:ny+1))
  allocate(edge(nx,ny))

  call ipgmread(infile, image(1:nx, 1:ny))
  
! periodic bcs on top and bottom

  image(:,0)    = image(:,ny)
  image(:,ny+1) = image(:,1)

  do j = 1, ny

! compute sawtooth value on left and right
     
     val = boundaryval(j,ny)

     image(0,j)    = dble(thresh)*(1.0-val)
     image(nx+1,j) = dble(thresh)*val

  end do

  write(*,*) 'pgmread: image min, max = ', minval(image(1:nx, 1:ny)), &
                                           maxval(image(1:nx, 1:ny))

  do j = 1,ny
    do i = 1, nx

      edge(i,j) = image(i+1,j) + image(i-1,j) + image(i,j+1) + image(i,j-1) &
                  - 4.0*image(i,j)

    end do
  end do

  call ipgmwrite(outfile, edge)

  deallocate(image)
  deallocate(edge)

  write(*,*) 'pgm2edge: finished'

end program pgm2edge