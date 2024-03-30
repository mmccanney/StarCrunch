Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
   Friend WithEvents Board As System.Windows.Forms.TextBox
   Friend WithEvents txtScore As System.Windows.Forms.Label
   Friend WithEvents Timer1 As System.Windows.Forms.Timer
   <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
      Me.components = New System.ComponentModel.Container
      Me.Board = New System.Windows.Forms.TextBox
      Me.txtScore = New System.Windows.Forms.Label
      Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
      Me.SuspendLayout()
      '
      'Board
      '
      Me.Board.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
      Me.Board.BackColor = System.Drawing.Color.Black
      Me.Board.Font = New System.Drawing.Font("Courier New", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
      Me.Board.ForeColor = System.Drawing.Color.Lime
      Me.Board.Location = New System.Drawing.Point(16, 48)
      Me.Board.Multiline = True
      Me.Board.Name = "Board"
      Me.Board.ReadOnly = True
      Me.Board.Size = New System.Drawing.Size(504, 424)
      Me.Board.TabIndex = 0
      Me.Board.TabStop = False
      Me.Board.Text = "TextBox1"
      Me.Board.WordWrap = False
      '
      'txtScore
      '
      Me.txtScore.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
      Me.txtScore.Font = New System.Drawing.Font("Comic Sans MS", 15.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
      Me.txtScore.Location = New System.Drawing.Point(16, 8)
      Me.txtScore.Name = "txtScore"
      Me.txtScore.Size = New System.Drawing.Size(128, 32)
      Me.txtScore.TabIndex = 1
      Me.txtScore.Text = "Label1"
      Me.txtScore.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
      '
      'Timer1
      '
      '
      'Form1
      '
      Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
      Me.ClientSize = New System.Drawing.Size(536, 489)
      Me.Controls.Add(Me.txtScore)
      Me.Controls.Add(Me.Board)
      Me.KeyPreview = True
      Me.Name = "Form1"
      Me.Text = "Form1"
      Me.ResumeLayout(False)

   End Sub

#End Region
   REM compiled with Visual Studio 2003, Visual Basic, .Net 1.1
   Private Field(100, 80) As Char
   Private Stars(100, 80) As Char
   Private Display(8000) As Char
   Private ManPos As Integer = 15
   Private ManAdj As Integer = 0
   Private BulletPosX(100) As Integer
   Private BulletPosY(100) As Integer
   Private ManChar As Integer = 86
   Private BulletChar As Integer = 124
   Private BckGrnd As Integer = 32
   Private StarChar As Integer = 42
   Private BoardWidth As Integer = 50
   Private BoardHeight As Integer = 20
   Public Difficulty As Integer = 1
   Private Speed As Integer = 25
   Private ScanLevel As Integer = 1
   Private ScanLevel_Max As Integer = 3
   Private Score = 0
   Private State = 0    ' 0=running, 1=dead, 2=paused

   Private bFiring As Boolean = False
   Private bFireLE As Boolean = False
   Private MoveLE As Integer = 0
   Private Direction As Integer = 0

   Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
      ResetField()
      Timer1.Interval = Speed
      Timer1.Start()
   End Sub

   Private Sub MoveMan()
      ManPos = ManPos + ManAdj
      ManAdj = 0
   End Sub
   Private Sub MoveStars()
      Dim h, w, x, a As Integer
      ' move all lines up one
      For h = 1 To BoardHeight
         For w = 1 To BoardWidth
            Stars(h, w) = Stars(h + 1, w)
         Next
      Next

      ' create a new bottom line
      For w = 0 To BoardWidth
         Stars(BoardHeight, w) = Chr(BckGrnd)
      Next
      For x = 1 To Difficulty
         a = CInt(Int((BoardWidth * Rnd()) + 1)) ' Generate random value between 1 and BoardWidth
         Stars(BoardHeight, a) = Chr(StarChar)
      Next

   End Sub
   Private Sub MoveBullets()
      Dim x As Integer
      ' draw any bullets
      For x = 1 To 10
         If BulletPosY(x) > 1 Then
            BulletPosY(x) = BulletPosY(x) + 1
            If BulletPosY(x) > BoardHeight Then
               BulletPosY(x) = 0
            End If
         End If
      Next

   End Sub
   Public Sub ReDraw()
      Dim x, h, w, ptr, pos As Integer

      If State = 0 Then
         ' draw the stars onto the field
         ptr = 0
         For h = 1 To BoardHeight
            For w = 1 To BoardWidth
               Field(h, w) = Stars(h, w)
            Next
         Next

         ' draw the bullets onto the field
         For x = 1 To 100
            If BulletPosY(x) > 1 Then
               If Field(BulletPosY(x), BulletPosX(x)) = Chr(StarChar) Then
                  ' bullet hit a star - anihilate the two
                  Field(BulletPosY(x), BulletPosX(x)) = Chr(BckGrnd)
                  Stars(BulletPosY(x), BulletPosX(x)) = Chr(BckGrnd)
                  BulletPosY(x) = 0
                  BulletPosX(x) = 0
                  Score = Score + (Difficulty * 2)
               ElseIf Field(BulletPosY(x) + 1, BulletPosX(x)) = Chr(StarChar) Then
                  ' bullet hit a star - anihilate the two
                  Field(BulletPosY(x) + 1, BulletPosX(x)) = Chr(BckGrnd)
                  Stars(BulletPosY(x) + 1, BulletPosX(x)) = Chr(BckGrnd)
                  BulletPosY(x) = 0
                  BulletPosX(x) = 0
                  Score = Score + (Difficulty * 2)
               Else
                  Field(BulletPosY(x), BulletPosX(x)) = Chr(BulletChar)
               End If
               Field(BulletPosY(x), BulletPosX(x)) = Chr(BulletChar)
            End If
         Next

         ' draw the dude
         If Field(1, ManPos) <> Chr(BckGrnd) Then
            State = 1
            Timer1.Stop()
         Else
            Field(1, ManPos) = Chr(ManChar)
         End If

         ' draw the the display
         ptr = 0
         For h = 1 To BoardHeight
            For w = 1 To BoardWidth
               Display(ptr) = Field(h, w)
               ptr = ptr + 1
            Next
            Display(ptr) = vbCr
            ptr = ptr + 1
            Display(ptr) = vbLf
            ptr = ptr + 1
         Next
         ' update the score
         Score = Score + Difficulty

      ElseIf State = 2 Then
         ' game paused
         Display(0) = "P"
         Display(1) = "A"
         Display(2) = "U"
         Display(3) = "S"
         Display(4) = "E"
         Display(5) = "D"
      Else
         Display(0) = "G"
         Display(1) = "A"
         Display(2) = "M"
         Display(3) = "E"
         Display(4) = " "
         Display(5) = "O"
         Display(6) = "V"
         Display(7) = "E"
         Display(8) = "R"
         Display(9) = " "
         Display(10) = "M"
         Display(11) = "A"
         Display(12) = "N"
      End If

      txtScore.Text = Score.ToString

      Board.Text = Display
   End Sub
   Private Sub ResetField()
      ' clear the field
      Dim h, w As Integer
      For h = 1 To BoardHeight
         For w = 1 To BoardWidth
            Field(h, w) = Chr(BckGrnd)
            Stars(h, w) = Chr(BckGrnd)
         Next
      Next
      ' erase all bullets
      For h = 1 To 100
         BulletPosX(h) = 0
         BulletPosY(h) = 0
      Next
      ' center the dude
      ManPos = BoardWidth / 2

      Score = 0
      State = 0
      ScanLevel = 1
      Randomize()

   End Sub
   Private Sub Timer1_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles Timer1.Tick
      Timer1.Stop()

      If State = 0 Then
         ScanLevel = ScanLevel + 1
         If ScanLevel > ScanLevel_Max Then ScanLevel = 1


         If ScanLevel = 1 Then
            If bFiring Or bFireLE Then
               bFireLE = False
               Dim x As Integer
               ' find an empty bullet slot
               For x = 1 To 100
                  If BulletPosY(x) = 0 Then
                     BulletPosY(x) = 2
                     BulletPosX(x) = ManPos
                     Exit For
                  End If
               Next
            End If
            If MoveLE <> 0 Then
               ManAdj = MoveLE
               MoveLE = 0
               MoveMan()
            ElseIf Direction <> 0 Then
               ManAdj = Direction
               MoveMan()
            End If

            MoveStars()
         End If

         ' every scan cycle we move the bullets and draw the screen
         MoveBullets()
         ReDraw()

         Timer1.Start()
      Else
         ' game over man
         ReDraw()
      End If

   End Sub

   Private Sub Form1_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
      ' Harder - more stars
      If e.KeyCode = Keys.H Then
         Difficulty = Difficulty + 1
         If Difficulty > 10 Then Difficulty = 10
         e.Handled = True
      End If
      ' Easier - less stars
      If e.KeyCode = Keys.E Then
         Difficulty = Difficulty - 1
         If Difficulty < 1 Then Difficulty = 1
         e.Handled = True
      End If
      ' New Game
      If e.KeyCode = Keys.N Then
         ResetField()
         Timer1.Start()

         e.Handled = True
      End If
      ' Pause
      If e.KeyCode = Keys.P Then
         If State = 0 Then
            State = 2
         Else
            State = 0
            Timer1.Start()
         End If
         e.Handled = True
      End If
      ' Quit
      If e.KeyCode = Keys.Q Then
         State = 1
         e.Handled = True
      End If
      ' Left
      If e.KeyCode = Keys.Left Then
         MoveLE = -1       ' this ensures that we move even if we just tap the key really fast
         Direction = -1
         e.Handled = True
      End If
      ' Right
      If e.KeyCode = Keys.Right Then
         MoveLE = 1        ' this ensures that we move even if we just tap the key really fast
         Direction = 1
         e.Handled = True
      End If
      ' Fire
      If e.KeyCode = Keys.F Then
         bFireLE = True   ' ensures that a bullet is produced on a quick tap
         bFiring = True
         e.Handled = True
      End If

   End Sub
   Private Sub Form1_KeyUp(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyUp
      If e.KeyCode = Keys.Left Or e.KeyCode = Keys.Right Then
         Direction = 0
         e.Handled = True
      End If
      If e.KeyCode = Keys.F Then
         bFiring = False
         e.Handled = True
      End If
   End Sub
End Class
