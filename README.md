# Parent-Child-Process

#### İşletim Sistemleri

Parent-Child ilişkisi olan iki proses yaratıldı. Parent proses Ordinary Pipe kullanarak şifrelemek istediği metni Child prosese 
gönderecek. Child proses gelen metini Sezar Şifreleme yöntemi kullanarak değiştirecek. Child proses şifreleme işlemini 4 thread kullanarak (paralel bir şekilde) yapar. Kullanılan 4 thread, metini eşit parçalara böler ve şifreleme işlemi gerçekleştirilir.Threadlerde ki şifreleme işlemi bittikten sonra 4’e bölünen şifreli metin birleştirilir ve Shared Memory kullanılarak ortak bir alana yazılır. Parent proses Shared Memory de ki şifrelenmiş veriyi okuyacak ve ekrana 
basacak.
